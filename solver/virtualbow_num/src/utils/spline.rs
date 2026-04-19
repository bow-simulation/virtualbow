use crate::utils::functions::sign;
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
            args.iter().copied().zip(vals.iter().copied()).map(|(a, b)| [a, b]),
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
        let (x, y): (Vec<f64>, Vec<f64>) = points.into_iter().map(|[a, b]| (a, b)).unzip();
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
                if sign(m[i]) != sign(delta) {
                    m[i] = 0.0;
                }
                if sign(m[i+1]) != sign(delta) {
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
        *self.x.first().unwrap()  // Unwrap ensured by construction
    }

    pub fn arg_max(&self) -> f64 {
        *self.x.last().unwrap()  // Unwrap ensured by construction
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
    use approx::assert_abs_diff_eq;
    use super::*;
    use iter_num_tools::lin_space;

    #[test]
    fn test_linear_interpolation() {
        // Make sure that two control points with zero second derivative produce a straight line
        let x0 = 1.0;
        let y0 = 0.5;
        let x1 = 5.0;
        let y1 = 2.5;

        let spline = CubicSpline::from_points(&[[x0, y0], [x1, y1]], false, BoundaryCondition::SecondDerivative(0.0), BoundaryCondition::SecondDerivative(0.0));
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
    fn test_monotonic_interpolation_1() {
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
    fn test_monotonic_interpolation_2() {
        // Compare with monotonic interpolation results of previous implementation from VirtualBow 0.9.1
        let x = &[0.0, 0.069, 0.1381, 0.2071, 0.2486, 1.0];
        let y = &[0.04425, 0.0225, 0.0101, 0.0025, 0.0011, 0.0011];

        let x_ref = &[0.00000000, 0.00200401, 0.00400802, 0.00601202, 0.00801603, 0.0100200, 0.0120240, 0.0140281, 0.0160321, 0.0180361, 0.0200401, 0.0220441, 0.0240481, 0.0260521, 0.0280561, 0.0300601, 0.0320641, 0.0340681, 0.0360721, 0.0380762, 0.0400802, 0.0420842, 0.0440882, 0.0460922, 0.0480962, 0.0501002, 0.0521042, 0.0541082, 0.0561122, 0.0581162, 0.0601202, 0.0621242, 0.0641283, 0.0661323, 0.0681363, 0.0701403, 0.0721443, 0.0741483, 0.0761523, 0.0781563, 0.0801603, 0.0821643, 0.0841683, 0.0861723, 0.0881764, 0.0901804, 0.0921844, 0.0941884, 0.0961924, 0.0981964, 0.100200, 0.102204, 0.104208, 0.106212, 0.108216, 0.110220, 0.112224, 0.114228, 0.116232, 0.118236, 0.120240, 0.122244, 0.124248, 0.126253, 0.128257, 0.130261, 0.132265, 0.134269, 0.136273, 0.138277, 0.140281, 0.142285, 0.144289, 0.146293, 0.148297, 0.150301, 0.152305, 0.154309, 0.156313, 0.158317, 0.160321, 0.162325, 0.164329, 0.166333, 0.168337, 0.170341, 0.172345, 0.174349, 0.176353, 0.178357, 0.180361, 0.182365, 0.184369, 0.186373, 0.188377, 0.190381, 0.192385, 0.194389, 0.196393, 0.198397, 0.200401, 0.202405, 0.204409, 0.206413, 0.208417, 0.210421, 0.212425, 0.214429, 0.216433, 0.218437, 0.220441, 0.222445, 0.224449, 0.226453, 0.228457, 0.230461, 0.232465, 0.234469, 0.236473, 0.238477, 0.240481, 0.242485, 0.244489, 0.246493, 0.248497, 0.250501, 0.252505, 0.254509, 0.256513, 0.258517, 0.260521, 0.262525, 0.264529, 0.266533, 0.268537, 0.270541, 0.272545, 0.274549, 0.276553, 0.278557, 0.280561, 0.282565, 0.284569, 0.286573, 0.288577, 0.290581, 0.292585, 0.294589, 0.296593, 0.298597, 0.300601, 0.302605, 0.304609, 0.306613, 0.308617, 0.310621, 0.312625, 0.314629, 0.316633, 0.318637, 0.320641, 0.322645, 0.324649, 0.326653, 0.328657, 0.330661, 0.332665, 0.334669, 0.336673, 0.338677, 0.340681, 0.342685, 0.344689, 0.346693, 0.348697, 0.350701, 0.352705, 0.354709, 0.356713, 0.358717, 0.360721, 0.362725, 0.364729, 0.366733, 0.368737, 0.370741, 0.372745, 0.374749, 0.376754, 0.378758, 0.380762, 0.382766, 0.384770, 0.386774, 0.388778, 0.390782, 0.392786, 0.394790, 0.396794, 0.398798, 0.400802, 0.402806, 0.404810, 0.406814, 0.408818, 0.410822, 0.412826, 0.414830, 0.416834, 0.418838, 0.420842, 0.422846, 0.424850, 0.426854, 0.428858, 0.430862, 0.432866, 0.434870, 0.436874, 0.438878, 0.440882, 0.442886, 0.444890, 0.446894, 0.448898, 0.450902, 0.452906, 0.454910, 0.456914, 0.458918, 0.460922, 0.462926, 0.464930, 0.466934, 0.468938, 0.470942, 0.472946, 0.474950, 0.476954, 0.478958, 0.480962, 0.482966, 0.484970, 0.486974, 0.488978, 0.490982, 0.492986, 0.494990, 0.496994, 0.498998, 0.501002, 0.503006, 0.505010, 0.507014, 0.509018, 0.511022, 0.513026, 0.515030, 0.517034, 0.519038, 0.521042, 0.523046, 0.525050, 0.527054, 0.529058, 0.531062, 0.533066, 0.535070, 0.537074, 0.539078, 0.541082, 0.543086, 0.545090, 0.547094, 0.549098, 0.551102, 0.553106, 0.555110, 0.557114, 0.559118, 0.561122, 0.563126, 0.565130, 0.567134, 0.569138, 0.571142, 0.573146, 0.575150, 0.577154, 0.579158, 0.581162, 0.583166, 0.585170, 0.587174, 0.589178, 0.591182, 0.593186, 0.595190, 0.597194, 0.599198, 0.601202, 0.603206, 0.605210, 0.607214, 0.609218, 0.611222, 0.613226, 0.615230, 0.617234, 0.619238, 0.621242, 0.623246, 0.625251, 0.627255, 0.629259, 0.631263, 0.633267, 0.635271, 0.637275, 0.639279, 0.641283, 0.643287, 0.645291, 0.647295, 0.649299, 0.651303, 0.653307, 0.655311, 0.657315, 0.659319, 0.661323, 0.663327, 0.665331, 0.667335, 0.669339, 0.671343, 0.673347, 0.675351, 0.677355, 0.679359, 0.681363, 0.683367, 0.685371, 0.687375, 0.689379, 0.691383, 0.693387, 0.695391, 0.697395, 0.699399, 0.701403, 0.703407, 0.705411, 0.707415, 0.709419, 0.711423, 0.713427, 0.715431, 0.717435, 0.719439, 0.721443, 0.723447, 0.725451, 0.727455, 0.729459, 0.731463, 0.733467, 0.735471, 0.737475, 0.739479, 0.741483, 0.743487, 0.745491, 0.747495, 0.749499, 0.751503, 0.753507, 0.755511, 0.757515, 0.759519, 0.761523, 0.763527, 0.765531, 0.767535, 0.769539, 0.771543, 0.773547, 0.775551, 0.777555, 0.779559, 0.781563, 0.783567, 0.785571, 0.787575, 0.789579, 0.791583, 0.793587, 0.795591, 0.797595, 0.799599, 0.801603, 0.803607, 0.805611, 0.807615, 0.809619, 0.811623, 0.813627, 0.815631, 0.817635, 0.819639, 0.821643, 0.823647, 0.825651, 0.827655, 0.829659, 0.831663, 0.833667, 0.835671, 0.837675, 0.839679, 0.841683, 0.843687, 0.845691, 0.847695, 0.849699, 0.851703, 0.853707, 0.855711, 0.857715, 0.859719, 0.861723, 0.863727, 0.865731, 0.867735, 0.869739, 0.871743, 0.873747, 0.875752, 0.877756, 0.879760, 0.881764, 0.883768, 0.885772, 0.887776, 0.889780, 0.891784, 0.893788, 0.895792, 0.897796, 0.899800, 0.901804, 0.903808, 0.905812, 0.907816, 0.909820, 0.911824, 0.913828, 0.915832, 0.917836, 0.919840, 0.921844, 0.923848, 0.925852, 0.927856, 0.929860, 0.931864, 0.933868, 0.935872, 0.937876, 0.939880, 0.941884, 0.943888, 0.945892, 0.947896, 0.949900, 0.951904, 0.953908, 0.955912, 0.957916, 0.959920, 0.961924, 0.963928, 0.965932, 0.967936, 0.969940, 0.971944, 0.973948, 0.975952, 0.977956, 0.979960, 0.981964, 0.983968, 0.985972, 0.987976, 0.989980, 0.991984, 0.993988, 0.995992, 0.997996, 1.000000];
        let y_ref = &[0.0442500, 0.0435503, 0.0428509, 0.0421522, 0.0414545, 0.0407582, 0.0400637, 0.0393712, 0.0386811, 0.0379938, 0.0373096, 0.0366288, 0.0359518, 0.0352789, 0.0346106, 0.0339470, 0.0332886, 0.0326358, 0.0319888, 0.0313480, 0.0307137, 0.0300864, 0.0294662, 0.0288537, 0.0282491, 0.0276527, 0.0270650, 0.0264862, 0.0259168, 0.0253570, 0.0248071, 0.0242676, 0.0237388, 0.0232211, 0.0227147, 0.0222200, 0.0217370, 0.0212653, 0.0208046, 0.0203546, 0.0199149, 0.0194852, 0.0190650, 0.0186541, 0.0182521, 0.0178587, 0.0174735, 0.0170961, 0.0167263, 0.0163635, 0.0160076, 0.0156582, 0.0153149, 0.0149773, 0.0146451, 0.0143180, 0.0139956, 0.0136776, 0.0133636, 0.0130532, 0.0127462, 0.0124421, 0.0121406, 0.0118414, 0.0115441, 0.0112484, 0.0109539, 0.0106603, 0.0103671, 0.0100742, 0.00978118, 0.00948841, 0.00919624, 0.00890507, 0.00861527, 0.00832723, 0.00804132, 0.00775794, 0.00747746, 0.00720026, 0.00692673, 0.00665724, 0.00639219, 0.00613195, 0.00587690, 0.00562743, 0.00538391, 0.00514674, 0.00491628, 0.00469293, 0.00447707, 0.00426907, 0.00406933, 0.00387821, 0.00369611, 0.00352340, 0.00336047, 0.00320770, 0.00306547, 0.00293416, 0.00281416, 0.00270584, 0.00260959, 0.00252580, 0.00245088, 0.00237213, 0.00228934, 0.00220338, 0.00211512, 0.00202543, 0.00193518, 0.00184524, 0.00175647, 0.00166976, 0.00158597, 0.00150596, 0.00143062, 0.00136080, 0.00129738, 0.00124124, 0.00119323, 0.00115424, 0.00112512, 0.00110676, 0.00110002, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011, 0.0011];

        let spline = CubicSpline::from_components(x, y, true, BoundaryCondition::SecondDerivative(0.0), BoundaryCondition::SecondDerivative(0.0));

        for i in 0..x_ref.len() {
            let y_num = spline.value(x_ref[i], Extrapolation::Linear);
            let y_ref = y_ref[i];
            assert_abs_diff_eq!(y_num, y_ref, epsilon=1e-6);
        }
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

        let spline = CubicSpline::from_points(&[[x0, y0], [x1, y1], [x2, y2]], false, BoundaryCondition::SecondDerivative(0.0), BoundaryCondition::SecondDerivative(0.0));

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

        // Cubic extrapolation (reference values taken from previous run)
        assert_abs_diff_eq!(spline.value(x_min, Extrapolation::Cubic), -0.390625000000000, epsilon=1e-12);
        assert_abs_diff_eq!(spline.value(x_max, Extrapolation::Cubic), 0.624421296296296, epsilon=1e-12);
        assert_abs_diff_eq!(spline.deriv1(x_min, Extrapolation::Cubic), 1.927083333333334, epsilon=1e-12);
        assert_abs_diff_eq!(spline.deriv1(x_max, Extrapolation::Cubic), -1.670138888888889, epsilon=1e-12);
        assert_abs_diff_eq!(spline.deriv2(x_min, Extrapolation::Cubic), -0.875000000000000, epsilon=1e-12);
        assert_abs_diff_eq!(spline.deriv2(x_max, Extrapolation::Cubic), 0.486111111111111, epsilon=1e-12);
    }
}