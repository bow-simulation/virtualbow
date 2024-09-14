use super::element::Element;
use super::dof::Dof;
use super::nodes::{Node, PointNode, OrientedNode};
use super::views::{PositionView, VelocityView, VectorView, MatrixView};
use super::nodes::Constraints;
use nalgebra::{DMatrix, DVector, SVector};
use crate::fem::system::views::{AccelerationView, ForceView};

// The system struct holds the elements, external forces and their mapping to the dofs that make up the fem system.
// It also holds the current system state: time, displacements and velocities.
//
// The system provides several methods to evaluate itself for different use cases (statics, dynamics, eigenvalues).
// Each of those evaluation methods has a corresponding struct that it fills with the evaluation results, which are things like internal forces, mass-, damping- or stiffness matrices.
// The evaluation methods and structs are used by the various solvers in order to solve the statics/dynamics of the system and the eval structs can also be passed on to the caller
// of the solver in order to provide information that is only available after having evaluated the system at the current state, like e.g. forces and accelerations.
//
// In addition to the system's internal state and the evaluation structs there is a third place where state and results are stored, which are the elements themselves.
// Each time the system is evaluated, the elements are assigned local positions and velocities corresponding to the system state, from which they compute requested quantities,
// such as internal forces and mass-, damping- or stiffness matrices. Additionally the elements compute local results which are specific to the element type
// or store information that makes computing those results easy when queried later. A beam element, for example, might compute curvatures and bending moments.

type ElementHandle = Box<dyn Element>;
type ExternalForce = Box<dyn Fn(f64) -> f64>;

#[derive(Clone)]
pub struct StaticEval {
    λ: f64,             // Current load factor
    p: DVector<f64>,    // Vector of external forces
    q: DVector<f64>,    // Vector of internal forces
    K: DMatrix<f64>,    // Tangent stiffness matrix
}

impl StaticEval {
    pub fn get_load_factor(&self) -> f64 {
        self.λ
    }

    pub fn set_load_factor(&mut self, λ: f64) {
        self.λ = λ;
    }

    pub fn get_unscaled_external_forces(&self) -> &DVector<f64> {
        &self.p
    }

    pub fn get_scaled_external_force(&self, dof: Dof) -> f64 {
        self.λ*ForceView::transform(&self.p, dof)
    }

    pub fn get_internal_forces(&self) -> &DVector<f64> {
        &self.q
    }

    pub fn get_internal_force(&self, dof: Dof) -> f64 {
        ForceView::transform(&self.q, dof)
    }

    pub fn get_tangent_stiffness_matrix(&self) -> &DMatrix<f64> {
        &self.K
    }
}

#[derive(Clone)]
pub struct DynamicEval {
    M: DVector<f64>,    // Mass matrix
    K: DMatrix<f64>,    // Tangent stiffness matrix
    D: DMatrix<f64>,    // Tangent damping matrix
    p: DVector<f64>,    // External forces
    q: DVector<f64>,    // Internal forces
    a: DVector<f64>,    // Accelerations
}

impl DynamicEval {
    pub fn get_mass_matrix(&self) -> &DVector<f64> {
        &self.M
    }

    pub fn get_stiffness_matrix(&self) -> &DMatrix<f64> {
        &self.K
    }

    pub fn get_damping_matrix(&self) -> &DMatrix<f64> {
        &self.D
    }

    pub fn get_external_forces(&self) -> &DVector<f64> {
        &self.p
    }

    pub fn get_external_force(&self, dof: Dof) -> f64 {
        ForceView::transform(&self.p, dof)
    }

    pub fn get_internal_forces(&self) -> &DVector<f64> {
        &self.q
    }

    pub fn get_internal_force(&self, dof: Dof) -> f64 {
        ForceView::transform(&self.q, dof)
    }

    pub fn get_accelerations(&self) -> &DVector<f64> {
        &self.a
    }

    pub fn get_acceleration(&self, dof: Dof) -> f64 {
        AccelerationView::transform(&self.a, dof)
    }
}

#[derive(Clone)]
pub struct EigenEval {
    M: DVector<f64>,    // Mass matrix
    D: DMatrix<f64>,    // Tangent damping matrix
    K: DMatrix<f64>,    // Tangent stiffness matrix
}

impl EigenEval {
    pub fn get_mass_matrix(&self) -> &DVector<f64> {
        &self.M
    }

    pub fn get_tangent_damping_matrix(&self) -> &DMatrix<f64> {
        &self.D
    }

    pub fn get_tangent_stiffness_matrix(&self) -> &DMatrix<f64> {
        &self.K
    }
}

pub struct System {
    // Model elements
    elements: Vec<(Vec<Dof>, ElementHandle)>,    // List of elements with the dofs that connect them to the system
    forces: Vec<(Dof, ExternalForce)>,           // Externally applied forces, given as functions of time for each dof

    // System state
    t: f64,             // Current time
    u: DVector<f64>,    // Displacements
    v: DVector<f64>,    // Velocities
}

impl System {
    // Creates a new empty system with zero degrees of freedom.
    pub fn new() -> Self {
        Self {
            elements: Vec::new(),
            forces: Vec::new(),
            t: 0.0,
            u: DVector::zeros(0),
            v: DVector::zeros(0),
        }
    }

    // Adds an element to the system by specifying its nodes (connectivity) and the element itself.
    pub fn add_element<T: Element + Send + 'static>(&mut self, nodes: &[impl Node], element: T) -> usize {
        let mut dofs = Vec::new();
        for node in nodes {
            dofs.extend_from_slice(node.dofs());
        }

        self.elements.push((dofs, Box::new(element)));
        return self.elements.len() - 1;
    }

    // Adds an external force, which can be time-dependent, to the system.
    // It is specified as a function of time and the dof on which it acts (must not be a fixed dof).
    // The actual force vector at a certain time is later calculated by summing all added forces for each dof.
    pub fn add_force<T: Fn(f64) -> f64 + 'static>(&mut self, dof: Dof, force: T) {
        assert!(dof.is_free(), "Cannot apply external force to fixed dof");
        self.forces.push((dof, Box::new(force)));
    }

    // Creates a planar node with two degrees of freedom in x and y direction.
    pub fn create_xy_node(&mut self, x0: f64, y0: f64, constraints: Constraints) -> PointNode {
        let dof_x = if constraints.x_pos_fixed { self.create_fixed_dof(x0) } else { self.create_free_dof(x0, 1.0) };
        let dof_y = if constraints.y_pos_fixed { self.create_fixed_dof(y0) } else { self.create_free_dof(y0, 1.0) };

        PointNode::new(dof_x, dof_y)
    }

    // Creates a planar node with three degrees of freedom, two displacementd and a rotation.
    // Displacements: [x, y, φ]
    pub fn create_beam_node(&mut self, u: &SVector<f64, 3>, constraints: Constraints) -> OrientedNode {
        let dof_x = if constraints.x_pos_fixed { self.create_fixed_dof(u[0]) } else { self.create_free_dof(u[0], 1.0) };
        let dof_y = if constraints.y_pos_fixed { self.create_fixed_dof(u[1]) } else { self.create_free_dof(u[1], 1.0) };
        let dof_φ = if constraints.y_pos_fixed { self.create_fixed_dof(u[2]) } else { self.create_free_dof(u[2], 1.0) };

        OrientedNode::new(dof_x, dof_y, dof_φ)
    }

    // Creates a single unconstrained dof. Mainly used internally
    pub fn create_free_dof(&mut self, offset: f64, scale: f64) -> Dof {
        assert_ne!(scale, 0.0, "Dof scale must not be zero");

        self.u = self.u.push(0.0);
        self.v = self.v.push(0.0);

        Dof::Free {
            index: self.n_dofs() - 1,
            offset,
        }
    }

    // Created a single fixed dof. Mainly used internally.
    pub fn create_fixed_dof(&mut self, offset: f64) -> Dof {
        Dof::Fixed {
            offset
        }
    }

    // Returns the number of degrees of freedom of the system.
    pub fn n_dofs(&self) -> usize {
        self.u.len()
    }

    pub fn elements(&self) -> impl Iterator<Item=&ElementHandle> {
        self.elements.iter().map(|(_dofs, handle)| { handle })
    }

    pub fn element_ref<T: Element + 'static>(&self, index: usize) -> &T {
        let (_, element) = &self.elements[index];
        element.downcast_ref::<T>().expect("Bad element cast!")
    }

    pub fn element_mut<T: Element + 'static>(&mut self, index: usize) -> &mut T {
        let (_, element) = &mut self.elements[index];
        element.downcast_mut::<T>().expect("Bad element cast!")
    }

    pub fn get_time(&self) -> f64 {
        self.t
    }

    pub fn set_time(&mut self, t: f64) {
        self.t = t;
    }

    pub fn get_displacements(&self) -> &DVector<f64> {
        &self.u
    }

    pub fn get_displacement(&self, dof: Dof) -> f64 {
        PositionView::transform(&self.u, dof)
    }

    pub fn set_displacements(&mut self, value: &DVector<f64>) {
        self.u.copy_from(value);
    }

    pub fn get_velocities(&self) -> &DVector<f64> {
        &self.v
    }

    pub fn get_velocity(&self, dof: Dof) -> f64 {
        VelocityView::transform(&self.v, dof)
    }

    pub fn set_velocities(&mut self, value: &DVector<f64>) {
        self.v.copy_from(value);
    }

    pub fn create_static_eval(&self) -> StaticEval {
        let mut eval = StaticEval {
            λ: 1.0,
            p: DVector::<f64>::zeros(self.n_dofs()),
            q: DVector::<f64>::zeros(self.n_dofs()),
            K: DMatrix::<f64>::zeros(self.n_dofs(), self.n_dofs()),
        };

        // Initialize external forces, because it has to be done only once per static simulation
        eval.p.fill(0.0);
        for (dof, force) in &self.forces {
            let mut p_view = VectorView::new(&mut eval.p, std::slice::from_ref(dof));
            p_view.add(SVector::<f64, 1>::from_element(force(self.t)));
        }

        eval
    }

    pub fn create_dynamic_eval(&self) -> DynamicEval {
        // Create state with zero values
        let mut eval = DynamicEval {
            M: DVector::<f64>::zeros(self.n_dofs()),
            p: DVector::<f64>::zeros(self.n_dofs()),
            q: DVector::<f64>::zeros(self.n_dofs()),
            a: DVector::<f64>::zeros(self.n_dofs()),
            K: DMatrix::<f64>::zeros(self.n_dofs(), self.n_dofs()),
            D: DMatrix::<f64>::zeros(self.n_dofs(), self.n_dofs()),
        };

        // Initialize mass matrix, because it has to be done only once per dynamic simulation
        for (dofs, element) in &self.elements {
            let mut m_view = VectorView::new(&mut eval.M, dofs);
            element.evaluate_mass_matrix(&mut m_view);
        }

        eval
    }

    pub fn create_eigen_eval(&self) -> EigenEval {
        // Create state with zero values
        let mut eval = EigenEval {
            M: DVector::<f64>::zeros(self.n_dofs()),
            D: DMatrix::<f64>::zeros(self.n_dofs(), self.n_dofs()),
            K: DMatrix::<f64>::zeros(self.n_dofs(), self.n_dofs()),
        };

        // Initialize mass matrix, because it has to be done only once per dynamic simulation
        for (dofs, element) in &self.elements {
            let mut m_view = VectorView::new(&mut eval.M, dofs);
            element.evaluate_mass_matrix(&mut m_view);
        }

        eval
    }

    // TODO: Unify with other eval functions
    pub fn eval_statics(&mut self, eval: &mut StaticEval) {
        // Set to zero in case of previous values
        eval.q.fill(0.0);
        eval.K.fill(0.0);

        // Iterate over elements, set their state and add their contributions to the results
        for (dofs, element) in &mut self.elements {
            let u_view = PositionView::new(&self.u, dofs);
            let v_view = VelocityView::new(&self.v, dofs);

            let mut q_view = Some(VectorView::new(&mut eval.q, dofs));
            let mut k_view = Some(MatrixView::new(&mut eval.K, dofs));

            element.set_state_and_evaluate(&u_view, &v_view, q_view.as_mut(), k_view.as_mut(), None);
        }
    }

    // TODO: Unify with other eval functions
    pub fn eval_dynamics(&mut self, eval: &mut DynamicEval) {
        // Set to zero in case of previous values
        eval.K.fill(0.0);
        eval.D.fill(0.0);
        eval.p.fill(0.0);
        eval.q.fill(0.0);

        // Evaluate external forces
        for (dof, force) in &self.forces {
            let mut p_view = VectorView::new(&mut eval.p, std::slice::from_ref(dof));
            p_view.add(SVector::<f64, 1>::from_element(force(self.t)));
        }

        // Iterate over elements, set their state and add their contributions to the results
        for (dofs, element) in &mut self.elements {
            let u_view = PositionView::new(&self.u, dofs);
            let v_view = VelocityView::new(&self.v, dofs);

            let mut q_view = Some(VectorView::new(&mut eval.q, dofs));
            let mut k_view = Some(MatrixView::new(&mut eval.K, dofs));
            let mut d_view = Some(MatrixView::new(&mut eval.D, dofs));

            element.set_state_and_evaluate(&u_view, &v_view, q_view.as_mut(), k_view.as_mut(), d_view.as_mut());
        }

        // Evaluate accelerations
        eval.a.copy_from(&(&eval.p - &eval.q).component_div(&eval.M));
    }

    // TODO: Unify with other eval functions
    pub fn eval_eigen(&mut self, eval: &mut EigenEval) {
        // Set to zero in case of previous values
        eval.K.fill(0.0);
        eval.D.fill(0.0);

        // Iterate over elements, set their state and add their contributions to the results
        for (dofs, element) in &mut self.elements {
            let u_view = PositionView::new(&self.u, dofs);
            let v_view = VelocityView::new(&self.v, dofs);

            let mut k_view = Some(MatrixView::new(&mut eval.K, dofs));
            let mut d_view = Some(MatrixView::new(&mut eval.D, dofs));

            element.set_state_and_evaluate(&u_view, &v_view, None, k_view.as_mut(), d_view.as_mut());
        }
    }
}