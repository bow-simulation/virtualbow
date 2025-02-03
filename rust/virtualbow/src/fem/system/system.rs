use crate::fem::system::element::Element;
use crate::fem::system::dof::Dof;
use crate::fem::system::node::Node;
use crate::fem::system::views::{PositionView, VelocityView, VectorView, MatrixView, AccelerationView, ForceView};
use nalgebra::{DMatrix, DVector, SVector};

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
    pub fn add_element<E>(&mut self, nodes: &[Node], element: E) -> usize
        where E: Element + Send + 'static
    {
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

    // Removes all external forces
    pub fn clear_forces(&mut self) {
        self.forces.clear();
    }

    // Creates a planar node with three degrees of freedom, two displacements in x and y and a rotation angle.
    pub fn create_node(&mut self, u: &SVector<f64, 3>, free: &[bool; 3]) -> Node {
        let dof_x = if free[0] { self.create_free_dof(u[0], 0.0) } else { Dof::Fixed(u[0]) };
        let dof_y = if free[1] { self.create_free_dof(u[1], 0.0) } else { Dof::Fixed(u[1]) };
        let dof_φ = if free[2] { self.create_free_dof(u[2], 0.0) } else { Dof::Fixed(u[2]) };

        Node::new(dof_x, dof_y, dof_φ)
    }

    // Creates a single unconstrained dof with initial position and velocity. Mainly used internally.
    pub fn create_free_dof(&mut self, u: f64, v: f64) -> Dof {
        self.u = self.u.push(u);
        self.v = self.v.push(v);

        Dof::Free(self.n_dofs() - 1)
    }

    // Returns the number of degrees of freedom of the system.
    pub fn n_dofs(&self) -> usize {
        self.u.len()
    }

    #[allow(dead_code)]
    pub fn elements(&self) -> impl Iterator<Item=&ElementHandle> {
        self.elements.iter().map(|(_dofs, handle)| { handle })
    }

    pub fn element_ref<T: Element + 'static>(&self, index: usize) -> &T {
        let (_, element) = &self.elements[index];
        element.downcast_ref::<T>().expect("Invalid element cast!")
    }

    pub fn element_mut<T: Element + 'static>(&mut self, index: usize) -> &mut T {
        let (_, element) = &mut self.elements[index];
        element.downcast_mut::<T>().expect("Invalid element cast!")
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

    pub fn compute_mass_matrix(&self, output: &mut DVector<f64>) {
        assert_eq!(output.len(), self.n_dofs(), "Dimension must match the number of degrees of freedom");
        output.fill(0.0);

        for (dofs, element) in &self.elements {
            let mut view = VectorView::new(output, dofs);
            element.evaluate_mass_matrix(&mut view);
        }
    }

    pub fn compute_external_forces(&self, output: &mut DVector<f64>) {
        assert_eq!(output.len(), self.n_dofs(), "Dimension must match the number of degrees of freedom");
        output.fill(0.0);

        for (dof, force) in &self.forces {
            let mut view = VectorView::new(output, std::slice::from_ref(dof));
            view.add_vec(SVector::<f64, 1>::from_element(force(self.t)));
        }
    }

    // Evaluates the element with the current system state
    // Only necessary in special occasions, usually the solvers do this anyway.
    // TODO: Do this when elements are added to the system
    pub fn eval_element(&mut self, index: usize) {
        let (dofs, element) = &mut self.elements[index];
        let u_view = PositionView::new(&self.u, dofs);
        let v_view = VelocityView::new(&self.v, dofs);

        element.set_state_and_evaluate(&u_view, &v_view, None, None, None);
    }

    pub fn compute_internal_forces(&mut self, mut q: Option<&mut DVector<f64>>, mut K: Option<&mut DMatrix<f64>>, mut D: Option<&mut DMatrix<f64>>) {
        // Initialize results with zero, if present
        if let Some(q) = &mut q { q.fill(0.0); }
        if let Some(K) = &mut K { K.fill(0.0); }
        if let Some(D) = &mut D { D.fill(0.0); }

        // Iterate over elements, set their state and add their contributions to the results
        for (dofs, element) in &mut self.elements {
            let u_view = PositionView::new(&self.u, dofs);
            let v_view = VelocityView::new(&self.v, dofs);

            let mut q_view = q.as_mut().map(|q|{ VectorView::new(q, dofs) });
            let mut k_view = K.as_mut().map(|K|{ MatrixView::new(K, dofs) });
            let mut d_view = D.as_mut().map(|D|{ MatrixView::new(D, dofs) });

            // TODO: Maybe include a separate element.set_state(u, v) step before. Then make eval_element method obsolete.
            element.set_state_and_evaluate(&u_view, &v_view, q_view.as_mut(), k_view.as_mut(), d_view.as_mut());
        }
    }
}

// Wrapper around force-, and acceleration vectors.
// Used by the different solvers to give access to those quantities in a unified way.
pub struct SystemEval<'a> {
    p: &'a DVector<f64>,    // External forces
    q: &'a DVector<f64>,    // Internal forces
    a: &'a DVector<f64>,    // Accelerations
}

impl<'a> SystemEval<'a> {
    pub fn new(p: &'a DVector<f64>, q: &'a DVector<f64>, a: &'a DVector<f64>) -> Self {
        Self {
            p,
            q,
            a
        }
    }

    pub fn get_external_force(&self, dof: Dof) -> f64 {
        ForceView::transform(self.p, dof)
    }

    #[allow(dead_code)]
    pub fn get_internal_force(&self, dof: Dof) -> f64 {
        ForceView::transform(self.q, dof)
    }

    pub fn get_acceleration(&self, dof: Dof) -> f64 {
        AccelerationView::transform(self.a, dof)
    }
}