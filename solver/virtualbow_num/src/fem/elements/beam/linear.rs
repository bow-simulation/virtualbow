use iter_num_tools::lin_space;
use itertools::Itertools;
use nalgebra::{matrix, SMatrix, stack, SVector, vector};
use serde::{Deserialize, Serialize};
use crate::fem::elements::beam::geometry::{CrossSection, PlanarCurve};
use crate::utils::integration::fixed_simpson;

// This module contains the necessary methods for determining the linear properties of a beam section

#[derive(Serialize, Deserialize, Default, PartialEq, Debug, Clone)]
pub struct LinearBeamSegment {
    pub s0: f64,                        // Start length
    pub s1: f64,                        // End length
    pub se: Vec<f64>,                   // Eval lengths

    pub p0: SVector<f64, 3>,            // Starting point (x, y, φ)
    pub p1: SVector<f64, 3>,            // Ending point (x, y, φ)
    pub pe: Vec<SVector<f64, 3>>,       // Eval points (x, y, φ)

    pub Ep: Vec<SMatrix<f64, 3, 6>>,    // Evaluation of displacements (x, y, phi)
    pub Ef: Vec<SMatrix<f64, 3, 6>>,    // Evaluation of section forces (N, Q, M)
    pub Ci: Vec<SMatrix<f64, 3, 3>>,    // Inverse cross-section stiffness (compliance)

    pub K: SMatrix<f64, 6, 6>,              // Stiffness matrix
    pub M: SVector<f64, 6>,                 // Lumped mass matrix
    pub m: f64                              // Total segment mass
}

impl LinearBeamSegment {
    // Discretizes a continuous geometry into a given number of linear beam segments
    // Returns segments, points, lengths
    // TODO: Specify (number of) eval points?
    pub fn discretize<C, S>(curve: &C, section: &S, n_elements: usize, n_eval_per_element: usize) -> (Vec<Self>, Vec<SVector<f64, 3>>, Vec<f64>)
    where C: PlanarCurve,
          S: CrossSection
    {
        assert!(n_elements >= 1, "At least one element required");

        let s_node = lin_space(curve.length_start()..=curve.length_end(), n_elements + 1).collect_vec();                            // Lengths at which the element nodes are placed

        let segments = s_node.iter().tuple_windows().map(|(&s0, &s1)| {
            let s_eval = lin_space(s0..=s1, n_eval_per_element).collect_vec();            // Lengths at which the elements are evaluated
            LinearBeamSegment::new(curve, section, s0, s1, &s_eval)
        }).collect_vec();

        // TODO: This is kind of ugly...
        let mut points = Vec::new();
        points.push(segments[0].p0);
        segments.iter().for_each(|s| points.push(s.p1));

        (segments, points, s_node)
    }

    pub fn new<C, S>(curve: &C, section: &S, s0: f64, s1: f64, se: &[f64]) -> Self
        where C: PlanarCurve,
              S: CrossSection
    {
        assert!(s0 < s1, "Starting length must be smaller than ending length");
        //assert!(s_eval.iter().all(|&s| s >= s0 && s <= s1), "Evaluation points must lie within start- and endpoint");  // TODO: Check with tolerance?

        // Fixed number of integration intervals
        let n_integration = 1000;

        // Segment starting point, endpoint and evaluation points
        let p0 = curve.point(s0);
        let p1 = curve.point(s1);
        let pe = se.iter().map(|&s| { curve.point(s) }).collect_vec();

        let H = |s, sn| -> SMatrix<f64, 3, 3> {
            let r = curve.position(s);
            let rn = curve.position(sn);
            let φ = curve.angle(s);

            matrix![
                f64::cos(φ), f64::sin(φ), 0.0;
                -f64::sin(φ), f64::cos(φ), 0.0;
                r[1] - rn[1], rn[0] - r[0], 1.0;
            ]
        };

        let B = |si, sj| -> SMatrix<f64, 3, 3> {
            let ri = curve.position(si);
            let rj = curve.position(sj);
            matrix![
                -1.0, 0.0, 0.0;
                0.0, -1.0, 0.0;
                rj[1] - ri[1], ri[0] - rj[0], -1.0;
            ]
        };

        let dIds = |s| -> SMatrix<f64, 3, 3> {
            let H0 = H(s, s0);
            let C_inv = section.stiffness(curve.normalize(s)).try_inverse().expect("Failed to invert section stiffness matrix");
            return H0.transpose()*C_inv*H0;
        };

        // Evaluate integral between each point of interest (segment bounds + eval points)
        // - Segment start to first eval point, if present
        // - Between each pair of eval points, if present
        // - Last eval point to segment end, if present

        // TODO: Nicer way to do this?
        let mut s_integ = Vec::with_capacity(2 + se.len());
        s_integ.push(s0);
        s_integ.extend_from_slice(se);
        s_integ.push(s1);

        let mut I = vec![SMatrix::<f64, 3, 3>::zeros()];
        s_integ.iter().tuple_windows().for_each(|(&sa, &sb)| {
            let last = I.last().unwrap();    // Unwrap okay because there is at least one element
            I.push(last + fixed_simpson(dIds, sa, sb, n_integration));
        });

        // Compute inverse stiffness matrices at nodes and eval points

        let K0n_inv = se.iter().enumerate().map(|(i, &sn)| { -B(s0, sn).transpose()*(I.last().unwrap() - I[i+1]) }).collect_vec();    // Unwrap okay because there is at least one element
        let K00_inv = I.last().unwrap();    // Unwrap okay because there is at least one element

        let K1n_inv = se.iter().enumerate().map(|(i, &sn)| { B(s0, sn).transpose()*I[i+1]*B(s0, s1) }).collect_vec();
        let K11_inv = B(s0, s1).transpose()*I.last().unwrap()*B(s0, s1);    // Unwrap okay because there is at least one element

        // Complete stiffness matrix

        let K00 = K00_inv.try_inverse().expect("Failed to invert partial stiffness matrix");
        let K11 = K11_inv.try_inverse().expect("Failed to invert partial stiffness matrix");
        let K10 = B(s1, s0)*K00;
        let K01 = K10.transpose();

        let K = stack![
            K00, K01;
            K10, K11
        ];

        // Evaluation matrices

        let Ep = se.iter().enumerate().map(|(i, _)| {
            stack![K0n_inv[i]*K00, K1n_inv[i]*K11]
        }).collect();

        let Ef = se.iter().map(|&s| {
            let H1 = H(s, s1);  // TODO: Use already computed values for H here?
            stack![H1*K10, H1*K11]
        }).collect();

        let Ci = se.iter().map(|&s| {
            section.stiffness(curve.normalize(s)).try_inverse().expect("Failed to invert section stiffness matrix")
        }).collect();

        // Lumped mass matrix

        let sm = 0.5*(s0 + s1);  // Segment midpoint
        let m0 = fixed_simpson(|s|{ vector![ section.mass(curve.normalize(s))[(0, 0)] ] }, s0, sm, n_integration)[0];    // Mass of the first segment half
        let m1 = fixed_simpson(|s|{ vector![ section.mass(curve.normalize(s))[(0, 0)] ] }, sm, s1, n_integration)[0];    // Mass of the second segment half
        let J0 = 0.5*(s1 - s0)*section.mass(curve.normalize(s0))[(2, 2)];    // Lumped rotary inertia of the first node
        let J1 = 0.5*(s1 - s0)*section.mass(curve.normalize(s1))[(2, 2)];    // Lumped rotary inertia of the second node

        let M = vector![
            m0,
            m0,
            J0,
            m1,
            m1,
            J1,
        ];

        // Total segment mass
        let m = m0 + m1;

        /*
        // Alternative: integrating both mass and rotary inertia at the nodes only
        // TODO: Test against natural frequencies and decide
        let M = 0.5*(s1 - s0)*vector![
            section.mass(s0)[(0, 0)],
            section.mass(s0)[(0, 0)],
            section.mass(s0)[(2, 2)],
            section.mass(s1)[(0, 0)],
            section.mass(s1)[(0, 0)],
            section.mass(s1)[(2, 2)]
        ];
        */

        Self {
            s0,
            s1,
            se: se.to_vec(),
            p0,
            p1,
            pe,
            Ep,
            Ef,
            Ci,
            K,
            M,
            m
        }
    }
}

#[cfg(test)]
mod tests {
    use std::ops::AddAssign;
    use approx::{assert_abs_diff_eq, assert_relative_eq};
    use iter_num_tools::lin_space;
    use nalgebra::{DMatrix, matrix, SMatrix, stack};
    use crate::fem::elements::beam::geometry::{ArcCurve, CrossSection, LineCurve, PlanarCurve, RectangularSection};
    use crate::fem::elements::beam::linear::LinearBeamSegment;

    #[test]
    fn test_linear_stiffness_matrix_straight() {
        // Computes the stiffness matrix of a straight beam segment with constant cross section
        // and an arbitrary starting point and -angle and compares it to the exact solution
        // as well as the fem approximation.

        let angle = 0.1;
        let length = 0.8;
        let curve = LineCurve { x: 1.5, y: 2.0, φ: angle, l: length };
        let section = RectangularSection { w0: 0.01, h0: 0.01, w1: 0.01, h1: 0.01, ρ: 7850.0, E: 210e9, G: 80e9 };

        let n_elements = 100;
        let segment_fem = LinearBeamSegmentFEM::new(&curve, &section, 0.0, curve.length(), n_elements);
        let segment_num = LinearBeamSegment::new(&curve, &section, 0.0, curve.length(), &segment_fem.s_eval);

        let C_sec = section.stiffness(0.0);
        let K_ref = LinearBeamSegmentFEM::element_stiffness_matrix(C_sec[(0, 0)], C_sec[(1, 1)], C_sec[(2, 2)], curve.length(), angle);

        // Check stiffness matrices
        assert_relative_eq!(segment_num.K, K_ref, max_relative=1e-9);
        assert_relative_eq!(segment_fem.K, K_ref, max_relative=1e-6);

        // Check displacement evaluation matrices
        for i in 0..=n_elements {
            assert_abs_diff_eq!(segment_fem.u_eval[i], segment_num.Ep[i], epsilon=1e-6);
        }

        // Check total segment mass
        assert_abs_diff_eq!(segment_num.m, section.ρ*section.w0*section.h0*length, epsilon=1e-12);
    }

    #[test]
    fn test_linear_stiffness_matrix_curved() {
        // Computes the stiffness matrix of a curved beam segment with constant cross section
        // and an arbitrary starting point and -angle and compares it to the fem approximation.

        let length = 0.8;
        let curve = ArcCurve { x: 1.5, y: 2.0, φ: 0.1, l: length, r: 0.4 };
        let section = RectangularSection { w0: 0.01, h0: 0.01, w1: 0.01, h1: 0.01, ρ: 7850.0, E: 210e9, G: 80e9 };

        let n_elements = 100;
        let segment_fem = LinearBeamSegmentFEM::new(&curve, &section, 0.0, curve.length(), n_elements);
        let segment_num = LinearBeamSegment::new(&curve, &section, 0.0, curve.length(), &segment_fem.s_eval);

        // Check stiffness matrices
        assert_relative_eq!(segment_fem.K, segment_num.K, max_relative=1e-3);    // TODO: Precision?

        // Check displacement evaluation matrices
        for i in 0..=n_elements {
            assert_abs_diff_eq!(segment_fem.u_eval[i], segment_num.Ep[i], epsilon=1e-3);    // TODO: Precision?
        }

        // Check total segment mass
        assert_abs_diff_eq!(segment_num.m, section.ρ*section.w0*section.h0*length, epsilon=1e-12);
    }

    #[test]
    fn test_linear_stiffness_matrix_curved_nonuniform() {
        // Computes the stiffness matrix of a curved beam segment with varying cross section
        // and an arbitrary starting point and -angle and compares it to the fem approximation.

        let length = 0.8;
        let curve = ArcCurve { x: 1.5, y: 2.0, φ: 0.1, l: length, r: 0.4 };
        let section = RectangularSection { w0: 0.01, h0: 0.01, w1: 0.005, h1: 0.005, ρ: 7850.0, E: 210e9, G: 80e9 };

        let n_elements = 100;
        let segment_fem = LinearBeamSegmentFEM::new(&curve, &section, 0.0, curve.length(), n_elements);
        let segment_num = LinearBeamSegment::new(&curve, &section, 0.0, curve.length(), &segment_fem.s_eval);

        // Check stiffness matrices
        assert_relative_eq!(segment_fem.K, segment_num.K, max_relative=1e-3);    // TODO: Precision?

        // Check displacement evaluation matrices
        for i in 0..=n_elements {
            assert_abs_diff_eq!(segment_fem.u_eval[i], segment_num.Ep[i], epsilon=1e-3);    // TODO: Precision?
        }

        // Check total segment mass
        // Analytical volume from truncated pyramid: https://de.wikipedia.org/wiki/Pyramidenstumpf
        let A0 = section.w0*section.h0;
        let A1 = section.w1*section.h1;
        assert_abs_diff_eq!(segment_num.m, section.ρ*length/3.0*(A0 + f64::sqrt(A0*A1) + A1), epsilon=1e-12);
    }

    // Approximates the stiffness matrix of a curved, non-uniform beam segment by using a number of straight elements
    // and reducing the resulting stiffness matrix to the relevant displacements (first and last node)

    pub struct LinearBeamSegmentFEM {
        K: SMatrix<f64, 6, 6>,
        s_eval: Vec<f64>,
        u_eval: Vec<SMatrix<f64, 3, 6>>
    }

    impl LinearBeamSegmentFEM {
        pub fn new<C, S>(curve: &C, section: &S, s0: f64, s1: f64, n_elements: usize) -> Self
            where C: PlanarCurve,
                  S: CrossSection
        {
            assert!(n_elements >= 2);

            let n_nodes = n_elements + 1;
            let n_dofs = 3*n_nodes;

            // Create total stiffness matrix for all nodes
            let mut K_full = DMatrix::zeros(n_dofs, n_dofs);

            let s: Vec<f64> = lin_space(s0..=s1, n_nodes).collect();
            for i in 0..n_elements {
                let r_prev = curve.position(s[i]);
                let r_next = curve.position(s[i+1]);

                let dx = r_next[0] - r_prev[0];
                let dy = r_next[1] - r_prev[1];

                // TODO: Assert that C are diagonal matrices
                let C_prev = section.stiffness((s[i] - s0)/(s1 - s0));
                let C_next = section.stiffness((s[i+1] - s0)/(s1 - s0));

                K_full.view_mut((3*i, 3*i), (6, 6)).add_assign(&Self::element_stiffness_matrix(
                    0.5*(C_prev[(0, 0)] + C_next[(0, 0)]),    // Average longitudinal stiffness
                    0.5*(C_prev[(1, 1)] + C_next[(1, 1)]),    // Average bending stiffness
                    0.5*(C_prev[(2, 2)] + C_next[(2, 2)]),    // Average shear stiffness
                    f64::hypot(dx, dy),                               // Element length
                    f64::atan2(dy, dx)                                // Element angle
                ));
            }

            /*
            Reducing the total stiffness matrix to forces and displacements of the first and last node.
            K_total is partitioned like this:

            | F1 |   | K11    K12    K13 |   | U1 |
            | 0  | = | K12^T  K22    K23 | * | U2 |
            | F3 |   | K13^T  K23^T  K33 |   | U3 |

            F1, U1: Forces and displacements of the first node
            F3, U3: Forces and displacements of the last node
            U2: Equilibrium displacements of all nodes inbetween.

            Second block equation:

            0 = K12^T*U1 + K22*U2 + K23*U3
              => U2 = -K22^(-1)*(K12^T*U1 + K23*U3)

            First and third equations:

            F1 = K11*U1 + K12*U2 + K13*U3
               = K11*U1 - K12*K22^(-1)*(K12^T*U1 + K23*U3) + K13*U3
               = (K11 - K12*K22^(-1)*K12^T)*U1 + (K13 - K12*K22^(-1)*K23)*U3

            F3 = K13^T*U1 + K23^T*U2 + K33*U3
               = K13^T*U1 - K23^T*K22^(-1)*(K12^T*U1 + K23*U3) + K33*U3
               = (K13^T - K23^T*K22^(-1)*K12^T)*U1 + (K33 - K23^T*K22^(-1)*K23)*U3

            Giving the reduced stiffness matrix:

            | F1 |   | K11 - K12*K22^(-1)*K12^T      K13 - K12*K22^(-1)*K23   |   | U1 |
            |    | = |                                                        | * |    |
            | F3 |   | K13^T - K23^T*K22^(-1)*K12^T  K33 - K23^T*K22^(-1)*K23 |   | U3 |
            */

            let K11 = K_full.view((0, 0), (3, 3));
            let K22 = K_full.view((3, 3), (n_dofs-6, n_dofs-6));
            let K33 = K_full.view((n_dofs - 3, n_dofs - 3), (3, 3));
            let K12 = K_full.view((0, 3), (3, n_dofs - 6));
            let K13 = K_full.view((0, n_dofs - 3), (3, 3));
            let K23 = K_full.view((3, n_dofs - 3), (n_dofs - 6, 3));

            let K22_inv = K22.try_inverse().expect("Failed to invert stiffness matrix");

            // TODO: Find a way to use stack![] here with a fixed size matrix as a result.
            let mut K_red = SMatrix::<f64, 6, 6>::zeros();
            K_red.fixed_view_mut::<3, 3>(0, 0).copy_from(&(K11 - K12*&K22_inv*K12.transpose()));
            K_red.fixed_view_mut::<3, 3>(0, 3).copy_from(&(K13 - K12*&K22_inv*K23));
            K_red.fixed_view_mut::<3, 3>(3, 0).copy_from(&(K13.transpose()-K23.transpose()*&K22_inv*K12.transpose()));
            K_red.fixed_view_mut::<3, 3>(3, 3).copy_from(&(K33-K23.transpose()*&K22_inv*K23));

            // Evaluation matrices for displacements of eliminated nodes
            let mut u_eval = Vec::new();

            // The displacement at s = s0 is equal to that of the left node
            u_eval.push(matrix![
                1.0, 0.0, 0.0, 0.0, 0.0, 0.0;
                0.0, 1.0, 0.0, 0.0, 0.0, 0.0;
                0.0, 0.0, 1.0, 0.0, 0.0, 0.0;
            ]);

            // The displacements inbetween are determined by the equation for U2
            let eval = stack![-&K22_inv*&K12.transpose(), -&K22_inv*K23];

            for i in 0..n_nodes-2 {
                u_eval.push(eval.fixed_view::<3, 6>(3*i, 0).into());
            }

            // The displacement at s = s1 is equal to that of the right node
            u_eval.push(matrix![
                0.0, 0.0, 0.0, 1.0, 0.0, 0.0;
                0.0, 0.0, 0.0, 0.0, 1.0, 0.0;
                0.0, 0.0, 0.0, 0.0, 0.0, 1.0;
            ]);

            Self {
                K: K_red,
                s_eval: s,
                u_eval
            }
        }

        // Returns the analytical stiffness matrix of a straight Timoshenko beam segment
        // with constant cross section and a rotation angle alpha against the x-axis.
        // TODO: Add a source for the matrices K and T below
        fn element_stiffness_matrix(EA: f64, GA: f64, EI: f64, l: f64, alpha: f64) -> SMatrix<f64, 6, 6> {
            let Φ = 12.0*EI/(GA*l*l);
            let K = matrix![
                EA/l,                          0.0,                        0.0, -EA/l,                          0.0,                        0.0;
                 0.0,    12.0*EI/(l*l*l*(1.0 + Φ)),     6.0*EI/(l*l*(1.0 + Φ)),   0.0,   -12.0*EI/(l*l*l*(1.0 + Φ)),     6.0*EI/(l*l*(1.0 + Φ));
                 0.0,       6.0*EI/(l*l*(1.0 + Φ)),   EI/l*(4.0 + Φ)/(1.0 + Φ),   0.0,      -6.0*EI/(l*l*(1.0 + Φ)),   EI/l*(2.0 - Φ)/(1.0 + Φ);
               -EA/l,                          0.0,                        0.0,  EA/l,                          0.0,                        0.0;
                 0.0,   -12.0*EI/(l*l*l*(1.0 + Φ)),    -6.0*EI/(l*l*(1.0 + Φ)),   0.0,    12.0*EI/(l*l*l*(1.0 + Φ)),    -6.0*EI/(l*l*(1.0 + Φ));
                 0.0,       6.0*EI/(l*l*(1.0 + Φ)),   EI/l*(2.0 - Φ)/(1.0 + Φ),   0.0,      -6.0*EI/(l*l*(1.0 + Φ)),   EI/l*(4.0 + Φ)/(1.0 + Φ);
            ];

            let T = matrix![
                f64::cos(alpha), f64::sin(alpha), 0.0,              0.0,             0.0, 0.0;
               -f64::sin(alpha), f64::cos(alpha), 0.0,              0.0,             0.0, 0.0;
                            0.0,             0.0, 1.0,              0.0,             0.0, 0.0;
                            0.0,             0.0, 0.0,  f64::cos(alpha), f64::sin(alpha), 0.0;
                            0.0,             0.0, 0.0, -f64::sin(alpha), f64::cos(alpha), 0.0;
                            0.0,             0.0, 0.0,              0.0,             0.0, 1.0;
            ];

            T.transpose()*K*T
        }
    }
}