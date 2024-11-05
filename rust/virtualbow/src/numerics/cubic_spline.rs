use super::bisection::bisect_right;
use super::tridiagonal::TDMatrix;

#[derive(Debug, Copy, Clone)]
pub enum BoundaryCondition {
    FirstDerivative(f64),
    SecondDerivative(f64)
}

#[derive(Debug, Copy, Clone)]
pub enum Extrapolation {
    Constant,
    Linear,
    Cubic
}

#[derive(Debug)]
pub struct CubicSpline {
    x: Vec<f64>,
    y: Vec<f64>,
    m: Vec<f64>,
}

impl CubicSpline {
    pub fn from_components(args: &[f64], vals: &[f64], monotonic: bool, bc_left: BoundaryCondition, bc_right: BoundaryCondition) -> Self {
        Self::from_iter(
            args.iter().zip(vals).map(|(&x, &y)| [x, y]),
            monotonic,
            bc_left,
            bc_right
        )
    }

    pub fn from_points(points: &[[f64; 2]], monotonic: bool, bc_left: BoundaryCondition, bc_right: BoundaryCondition) -> Self {
        Self::from_iter(
            points.iter().copied(),
            monotonic,
            bc_left,
            bc_right
        )
    }

    pub fn from_iter<I: IntoIterator<Item=[f64; 2]>>(points: I, monotonic: bool, bc_left: BoundaryCondition, bc_right: BoundaryCondition) -> Self {
        let (x, y): (Vec<f64>, Vec<f64>) = points.into_iter().map(|p| (p[0], p[1])).unzip();
        let n = x.len();

        assert!(n >= 2, "At least two data points are required");
        assert!(x.windows(2).all(|x| x[0] < x[1]), "Arguments must be strictly increasing");

        // Determine slopes for C2 spline
        // m starts as the right hand side and is transformed to the resulting slopes by solving the system in-place
        let mut m = vec![0.0; n];
        let mut A = TDMatrix::zeros(n);

        // Contuity conditions
        for i  in 1..n-1 {
            A.set(i, i-1, x[i] - x[i-1]);
            A.set(i, i  , 2.0*((x[i+1] - x[i]) + (x[i] - x[i-1])));
            A.set(i, i+1, x[i+1] - x[i]);
            m[i] = 3.0*((y[i+1] - y[i])/(x[i+1] - x[i])*(x[i] - x[i-1]) + (y[i] - y[i-1])/(x[i] - x[i-1])*(x[i+1] - x[i]));
        }

        // Left boundary condition
        match bc_left {
            BoundaryCondition::FirstDerivative(value) => {
                A.set(0, 0, 1.0);
                m[0] = value;
            },
            BoundaryCondition::SecondDerivative(value) => {
                A.set(0, 0, 4.0);
                A.set(0, 1, 2.0);
                m[0] = 6.0*(y[1] - y[0])/(x[1] - x[0]) - (x[1] - x[0])*value;
            }
        }

        // Right boundary condition
        match bc_right {
            BoundaryCondition::FirstDerivative(value) => {
                A.set(n-1, n-1, 1.0);
                m[n-1] = value;
            },
            BoundaryCondition::SecondDerivative(value) => {
                A.set(n-1, n-2, 2.0);
                A.set(n-1, n-1, 4.0);
                m[n-1] = 6.0*(y[n-1] - y[n-2])/(x[n-1] - x[n-2]) + (x[n-1] - x[n-2])*value;
            }
        }

        A.solve(&mut m);  // Solve in-place

        // Optionally adjust slopes to preserve monotonicity
        // TODO: Which method is this?
        if monotonic {
            for i in 0..n-1 {
                // Ensure that sgn(m_{i}) = sgn(m_{i+1}) = sgn(delta_{i})
                let delta = (y[i+1] - y[i])/(x[i+1] - x[i]);
                if f64::signum(m[i]) != f64::signum(delta) {
                    m[i] = 0.0;
                }
                if f64::signum(m[i+1]) != f64::signum(delta) {
                    m[i+1] = 0.0;
                }

                // Scale slopes to prevent overshoot
                let alpha = m[i]/delta;
                let beta = m[i+1]/delta;

                if alpha*alpha + beta*beta > 9.0 {
                    let tau = 3.0/f64::hypot(alpha, beta);
                    m[i] = tau*alpha*delta;
                    m[i+1] = tau*beta*delta;
                }
            }
        }

        Self {
            x,
            y,
            m
        }
    }

    pub fn args(&self) -> &[f64] {
        &self.x
    }

    pub fn vals(&self) -> &[f64] {
        &self.y
    }

    pub fn arg_min(&self) -> f64 {
        *self.x.first().unwrap()  // Ensured by construction
    }

    pub fn arg_max(&self) -> f64 {
        *self.x.last().unwrap()  // Ensured by construction
    }

    // Returns the interpolated value for the given argument and extrapolates on out of bounds access
    pub fn value(&self, arg: f64, ext: Extrapolation) -> f64 {
        let h_00 = |t| { t*(t*(2.0*t - 3.0)) + 1.0 };
        let h_10 = |t| { t*(1.0 + t*(t - 2.0)) };
        let h_01 = |t| { t*(t*(3.0 - 2.0*t)) };
        let h_11 = |t| { t*(t*(t - 1.0)) };

        let index = bisect_right(&self.x, arg);
        let dx = self.x[index + 1] - self.x[index];
        let t = (arg - self.x[index])/dx;

        // Extrapolation to the left
        if arg < self.arg_min() {
            match ext {
                Extrapolation::Constant => {
                    return self.y[index];
                },
                Extrapolation::Linear => {
                    return self.y[index] + self.m[index]*(arg - self.x[index])
                },
                Extrapolation::Cubic => {
                    // Regular evaluation below
                }
            };
        }

        // Extrapolation to the right
        if arg > self.arg_max() {
            match ext {
                Extrapolation::Constant => {
                    return self.y[index + 1];
                },
                Extrapolation::Linear => {
                    return self.y[index + 1] + self.m[index + 1]*(arg - self.x[index + 1]);
                },
                Extrapolation::Cubic => {
                    // Regular evaluation below
                }
            };
        }

        // Regular evaluation
        h_00(t)*self.y[index] + h_10(t)*dx*self.m[index] + h_01(t)*self.y[index + 1] + h_11(t)*dx*self.m[index + 1]
    }

    // Returns the first derivative for the given argument
    // Extrapolates on out of bounds access
    pub fn deriv1(&self, arg: f64, ext: Extrapolation) -> f64 {
        let dhdt_00 = |t| { t*(6.0*t - 6.0) };
        let dhdt_10 = |t| { t*(3.0*t - 4.0) + 1.0 };
        let dhdt_01 = |t| { t*(-6.0*t + 6.0) };
        let dhdt_11 = |t| { t*(3.0*t - 2.0) };

        let index = bisect_right(&self.x, arg);
        let dx = self.x[index + 1] - self.x[index];
        let t = (arg - self.x[index])/dx;

        // Extrapolation to the left
        if arg < self.arg_min() {
            match ext {
                Extrapolation::Constant => {
                    return 0.0;
                },
                Extrapolation::Linear => {
                    return self.m[index];
                }
                Extrapolation::Cubic => {
                    // Regular evaluation below
                }
            };
        }

        // Extrapolation to the right
        if arg > self.arg_max() {
            match ext {
                Extrapolation::Constant => {
                    return 0.0;
                },
                Extrapolation::Linear => {
                    return self.m[index + 1];
                }
                Extrapolation::Cubic => {
                    // Regular evaluation below
                }
            };
        }

        // Regular evaluation
        dhdt_00(t)/dx*self.y[index] + dhdt_10(t)*self.m[index] + dhdt_01(t)/dx*self.y[index + 1] + dhdt_11(t)*self.m[index + 1]
    }

    // Returns the second derivative for the given argument
    // Extrapolates on out of bounds access
    pub fn deriv2(&self, arg: f64, ext: Extrapolation) -> f64 {
        let dhdt2_00 = |t| { 12.0*t - 6.0 };
        let dhdt2_10 = |t| { 6.0*t - 4.0 };
        let dhdt2_01 = |t| { -12.0*t + 6.0 };
        let dhdt2_11 = |t| { 6.0*t - 2.0 };

        let index = bisect_right(&self.x, arg);
        let dx = self.x[index + 1] - self.x[index];
        let t = (arg - self.x[index])/dx;

        // Extrapolation to the left or right
        if arg < self.arg_min() || arg > self.arg_max() {
            match ext {
                Extrapolation::Constant | Extrapolation::Linear => {
                    return 0.0;
                },
                Extrapolation::Cubic => {
                    // Regular evaluation below
                }
            };
        }

        // Regular evaluation
        dhdt2_00(t)/(dx*dx)*self.y[index] + dhdt2_10(t)/dx*self.m[index] + dhdt2_01(t)/(dx*dx)*self.y[index + 1] + dhdt2_11(t)/dx*self.m[index + 1]
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use iter_num_tools::lin_space;

    #[test]
    fn test_linear_interpolation() {
        // Make sure that two control points with zero second derivative produce a straight line
        let x0 = 1.0;
        let y0 = 0.5;
        let x1 = 5.0;
        let y1 = 2.5;

        let spline = CubicSpline::from_points(&vec![[x0, y0], [x1, y1]], false, BoundaryCondition::SecondDerivative(0.0), BoundaryCondition::SecondDerivative(0.0));
        for x in lin_space(x0..=x1, 100) {
            let d_ref = (y1 - y0)/(x1 - x0);
            let y_ref = y0 + (x - x0)*d_ref;

            assert_abs_diff_eq!(spline.value(x, Extrapolation::Constant), y_ref, epsilon=1e-12);
            assert_abs_diff_eq!(spline.deriv1(x, Extrapolation::Constant), d_ref, epsilon=1e-12);
            assert_abs_diff_eq!(spline.deriv2(x, Extrapolation::Constant), 0.0, epsilon=1e-12);
        }
    }

    #[test]
    fn test_cubic_interpolation() {
        // Create some splines with the same model data but different boundary conditions, then compare values,
        // first and second derivatives to reference data generated with tk::spline (https://github.com/ttk592/spline/)

        let x = &[0.0, 20.0, 40.0, 60.0, 80.0, 100.0];
        let y = &[60.0, 70.0, 50.0, 30.0, 20.0, 20.0];

        // Natural boundary conditions: zero second derivative
        {
            let x_ref = &[0.0, 5.0, 10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0, 50.0, 55.0, 60.0, 65.0, 70.0, 75.0, 80.0, 85.0, 90.0, 95.0, 100.0];
            let y_ref = &[60.0000000000, 64.3503289474, 67.9605263158, 70.0904605263, 70.0000000000, 67.2203947368, 62.3684210526, 56.3322368421, 50.0000000000, 44.1118421053, 38.8157894737, 34.1118421053, 30.0000000000, 26.4884868421, 23.6184210526, 21.4391447368, 20.0000000000, 19.3092105263, 19.2105263158, 19.5065789474, 20.0000000000];
            let d1_ref = &[0.894736842105, 0.820723684211, 0.598684210526, 0.228618421053, -0.289473684211, -0.792763157895, -1.11842105263, -1.26644736842, -1.23684210526, -1.11842105263, -1.00000000000, -0.881578947368, -0.763157894737, -0.639802631579, -0.506578947368, -0.363486842105, -0.210526315789, -0.0723684210526, 0.0263157894737, 0.0855263157895, 0.105263157895];
            let d2_ref = &[0.0000000000000, -0.0296052631579, -0.0592105263158, -0.0888157894737, -0.118421052632, -0.0828947368421, -0.0473684210526, -0.0118421052632, 0.0236842105263, 0.0236842105263, 0.0236842105263, 0.0236842105263, 0.0236842105263, 0.0256578947368, 0.0276315789474, 0.0296052631579, 0.0315789473684, 0.0236842105263, 0.0157894736842, 0.00789473684211, 0.0000000000000];

            let spline = CubicSpline::from_components(x, y, false, BoundaryCondition::SecondDerivative(0.0), BoundaryCondition::SecondDerivative(0.0));
            for i in 0..x_ref.len() {
                assert_abs_diff_eq!(spline.value(x_ref[i], Extrapolation::Linear), y_ref[i], epsilon=1e-10);
                assert_abs_diff_eq!(spline.deriv1(x_ref[i], Extrapolation::Linear), d1_ref[i], epsilon=1e-10);
                assert_abs_diff_eq!(spline.deriv2(x_ref[i], Extrapolation::Linear), d2_ref[i], epsilon=1e-10);
            }
        }

        // Non-zero second derivatives
        {
            let x_ref = &[0.0, 5.0, 10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0, 50.0, 55.0, 60.0, 65.0, 70.0, 75.0, 80.0, 85.0, 90.0, 95.0, 100.0];
            let y_ref = &[60.0000000000, 37.9298744019, 40.6877990431, 55.6018241627, 70.0000000000, 74.0385765550, 69.1866028708, 59.7413277512, 50.0000000000, 43.2595693780, 38.8157894737, 34.9641148325, 30.0000000000, 23.0793959330, 16.8002392344, 14.6209629187, 20.0000000000, 33.7978468900, 46.4832535885, 45.9270334928, 20.0000000000];
            let d1_ref = &[-7.74162679426, -1.50882177033, 2.18959330144, 3.35361842105, 1.98325358852, -0.224581339713, -1.57296650718, -2.06190191388, -1.69138755981, -1.06160287081, -0.772727272727, -0.82476076555, -1.21770334928, -1.43525717703, -0.961124401914, 0.204694976077, 2.06220095694, 3.05263157895, 1.61722488038, -2.24401913876, -8.53110047847];
            let d2_ref = &[1.500000000000, 0.993122009569, 0.486244019139, -0.0206339712919, -0.527511961722, -0.355622009569, -0.183732057416, -0.0118421052632, 0.16004784689, 0.0918660287081, 0.0236842105263, -0.0444976076555, -0.112679425837, 0.0256578947368, 0.163995215311, 0.302332535885, 0.440669856459, -0.0444976076555, -0.52966507177, -1.01483253589, -1.50000000000];

            let spline = CubicSpline::from_components(x, y, false, BoundaryCondition::SecondDerivative(1.5), BoundaryCondition::SecondDerivative(-1.5));
            for i in 0..x_ref.len() {
                assert_abs_diff_eq!(spline.value(x_ref[i], Extrapolation::Linear), y_ref[i], epsilon=1e-10);
                assert_abs_diff_eq!(spline.deriv1(x_ref[i], Extrapolation::Linear), d1_ref[i], epsilon=1e-10);
                assert_abs_diff_eq!(spline.deriv2(x_ref[i], Extrapolation::Linear), d2_ref[i], epsilon=1e-10);
            }
        }

        // Non-zero first derivatives
        {
            let x_ref = &[0.0, 5.0, 10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0, 50.0, 55.0, 60.0, 65.0, 70.0, 75.0, 80.0, 85.0, 90.0, 95.0, 100.0];
            let y_ref = &[60.0000000000, 63.1436901914, 66.7165071770, 69.4310705742, 70.0000000000, 67.5254186603, 62.6674641148, 56.4757775120, 50.0000000000, 44.0983851675, 38.8636363636, 34.1970693780, 30.0000000000, 26.2372906699, 23.1279904306, 20.9546949761, 20.0000000000, 20.3274521531, 21.1244019139, 21.3591507177, 20.0000000000];
            let d1_ref = &[0.500000000000, 0.714563397129, 0.671650717703, 0.371261961722, -0.186602870813, -0.768241626794, -1.13995215311, -1.30173444976, -1.25358851675, -1.11034688995, -0.986842105263, -0.883074162679, -0.799043062201, -0.696620813397, -0.537679425837, -0.322218899522, -0.0502392344498, 0.146830143541, 0.137559808612, -0.0780502392344, -0.5000000000000];
            let d2_ref = &[0.0686602870813, 0.0171650717703, -0.0343301435407, -0.0858253588517, -0.137320574163, -0.0953349282297, -0.0533492822967, -0.0113636363636, 0.0306220095694, 0.0266746411483, 0.0227272727273, 0.0187799043062, 0.0148325358852, 0.0261363636364, 0.0374401913876, 0.0487440191388, 0.0600478468900, 0.0187799043062, -0.0224880382775, -0.0637559808612, -0.105023923445];

            let spline = CubicSpline::from_components(x, y, false, BoundaryCondition::FirstDerivative(0.5), BoundaryCondition::FirstDerivative(-0.5));
            for i in 0..x_ref.len() {
                assert_abs_diff_eq!(spline.value(x_ref[i], Extrapolation::Linear), y_ref[i], epsilon=1e-10);
                assert_abs_diff_eq!(spline.deriv1(x_ref[i], Extrapolation::Linear), d1_ref[i], epsilon=1e-10);
                assert_abs_diff_eq!(spline.deriv2(x_ref[i], Extrapolation::Linear), d2_ref[i], epsilon=1e-10);
            }
        }
    }

    #[test]
    fn test_monotonic_interpolation() {
        // Create a monotonic spline from a dataset prone to overshoot and verify that there
        // are no minima or maxima between the control points (i.e. no sign changes of the first derivative)
        let x = &[0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0];
        let y = &[5.0, 4.8, 0.2, 0.0, 0.0, 0.2, 4.8, 5.0];

        let spline = CubicSpline::from_components(x, y, true, BoundaryCondition::SecondDerivative(0.0), BoundaryCondition::SecondDerivative(0.0));
        x.windows(2).for_each(|w| {
            for x in lin_space(w[0]..=w[1], 100) {
                let d = spline.deriv1(x, Extrapolation::Linear);

                if x < 3.0 {
                    assert!(d <= 0.0);
                }
                else if x < 4.0 {
                    assert!(d == 0.0);
                }
                else if x < 7.0 {
                    assert!(d >= 0.0);
                }
            }
        })
    }

    #[test]
    fn test_extrapolation() {
        // Test the different extrapolation modes

        let x0 = 1.0;
        let y0 = 0.5;

        let x1 = 2.0;
        let y1 = 2.5;

        let x2 = 5.0;
        let y2 = 1.5;

        let x_min = 0.5;  // Out of bounds to the left
        let x_max = 5.5;  // Out of bounds to the right

        let spline = CubicSpline::from_points(&vec![[x0, y0], [x1, y1], [x2, y2]], false, BoundaryCondition::SecondDerivative(0.0), BoundaryCondition::SecondDerivative(0.0));

        // Constant extrapolation
        assert_abs_diff_eq!(spline.value(x_min, Extrapolation::Constant), y0, epsilon=1e-12);
        assert_abs_diff_eq!(spline.value(x_max, Extrapolation::Constant), y2, epsilon=1e-12);
        assert_abs_diff_eq!(spline.deriv1(x_min, Extrapolation::Constant), 0.0, epsilon=1e-12);
        assert_abs_diff_eq!(spline.deriv1(x_max, Extrapolation::Constant), 0.0, epsilon=1e-12);
        assert_abs_diff_eq!(spline.deriv2(x_min, Extrapolation::Constant), 0.0, epsilon=1e-12);
        assert_abs_diff_eq!(spline.deriv2(x_max, Extrapolation::Constant), 0.0, epsilon=1e-12);

        // Linear extrapolation
        assert_abs_diff_eq!(spline.value(x_min, Extrapolation::Linear), y0 + spline.deriv1(x0, Extrapolation::Constant)*(x_min - x0), epsilon=1e-12);
        assert_abs_diff_eq!(spline.value(x_max, Extrapolation::Linear), y2 + spline.deriv1(x2, Extrapolation::Constant)*(x_max - x2), epsilon=1e-12);
        assert_abs_diff_eq!(spline.deriv1(x_min, Extrapolation::Linear), spline.deriv1(x0, Extrapolation::Constant), epsilon=1e-12);
        assert_abs_diff_eq!(spline.deriv1(x_max, Extrapolation::Linear), spline.deriv1(x2, Extrapolation::Constant), epsilon=1e-12);
        assert_abs_diff_eq!(spline.deriv2(x_min, Extrapolation::Linear), 0.0, epsilon=1e-12);
        assert_abs_diff_eq!(spline.deriv2(x_max, Extrapolation::Linear), 0.0, epsilon=1e-12);

        // Cubic extrapolation
        // TODO
    }
}