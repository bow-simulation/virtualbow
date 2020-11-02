import numpy as np
from math import sin, cos, tan, acos, hypot, atan2, pi, floor
from scipy.optimize import minimize, NonlinearConstraint, approx_fprime
from scipy.interpolate import CubicSpline
from scipy import integrate
import matplotlib.pyplot as plt
from functools import partial

import nlopt

# Optimization options
NLOPT_ALGORITHM = nlopt.LD_SLSQP;
NLOPT_FTOL_REL = 1e-6;
NLOPT_FTOL_ABS = 1e-6;
NLOPT_CTOL_ABS = 1e-6;
NLOPT_MAXEVAL = 100;

class Point:
    def __init__(self, x, y, phi):
        self.x = x
        self.y = y
        self.phi = phi
        
    def plot(self):
        plt.plot([self.x], [self.y], marker='o', color='red')

class SegmentSpecs:
    def __init__(self, length=None, angle=None, delta_x=None, delta_y=None):
        self.length = length
        self.angle = angle
        self.delta_x = delta_x
        self.delta_y = delta_y

# https://en.wikipedia.org/wiki/Curvature
# https://en.wikipedia.org/wiki/Arc_length
# https://en.wikipedia.org/wiki/Differentiable_curve#arc-length_transform
# https://en.wikipedia.org/wiki/Tangential_angle
class Segment:
    def __init__(self, c):
        if len(c) != 8:
            raise ValueError('Number of coefficients is wrong.')
        
        self.c = c

    @staticmethod
    def n_coeffs():
        return 8

    @staticmethod
    def estimate_coeffs(point, specs):
        # Returns coefficients from four Bezier points
        # https://de.wikipedia.org/wiki/B%C3%A9zierkurve
        def from_bezier_points4(p0, p1, p2, p3):
            a0 = p0
            a1 = -3*p0 + 3*p1
            a2 = 3*p0 - 6*p1 + 3*p2
            a3 = -p0 + 3*p1 - 3*p2 + p3
            
            return [a0[0], a1[0], a2[0], a3[0], a0[1], a1[1], a2[1], a3[1]]
        
        # Returns coefficients from two Bezier points
        def from_bezier_points3(p0, p1, p2):
            a0 = p0
            a1 = -2*p0 + 2*p1
            a2 = p0 - 2*p1 + p2
            
            return [a0[0], a1[0], a2[0], 0, a0[1], a1[1], a2[1], 0]
        
        # Returns coefficients for a curve that interpolates four points
        def from_interpolation(p0, p1, p2, p3):
            A = np.array([
                [ 1.0,     0.0,     0.0,      0.0, 0.0,     0.0,     0.0,      0.0 ],
                [ 0.0,     0.0,     0.0,      0.0, 1.0,     0.0,     0.0,      0.0 ],
                [ 1.0, 1.0/3.0, 1.0/9.0, 1.0/27.0, 0.0,     0.0,     0.0,      0.0 ],
                [ 0.0,     0.0,     0.0,      0.0, 1.0, 1.0/3.0, 1.0/9.0, 1.0/27.0 ],
                [ 1.0, 2.0/3.0, 4.0/9.0, 8.0/27.0, 0.0,     0.0,     0.0,      0.0 ],
                [ 0.0,     0.0,     0.0,      0.0, 1.0, 2.0/3.0, 4.0/9.0, 8.0/27.0 ],
                [ 1.0,     1.0,     1.0,      1.0, 0.0,     0.0,     0.0,      0.0 ],
                [ 0.0,     0.0,     0.0,      0.0, 1.0,     1.0,     1.0,      1.0 ]
            ]);

            p = np.concatenate([p0, p1, p2, p3])
            c = np.linalg.solve(A, p)
            return c.tolist()
        
        def case_l_a_x_y(length, angle, delta_x, delta_y):
            return case_a_x_y(angle, delta_x, delta_y)
        
        def case_l_a_x(length, angle, delta_x):
            return case_a_x(angle, delta_x)
        
        def case_l_a_y(length, angle, delta_y):
            return case_a_y(angle, delta_y)
        
        def case_l_x_y(length, delta_x, delta_y):
            return case_x_y(delta_x, delta_y)
        
        def case_a_x_y(angle, delta_x, delta_y):
            p0 = np.array([point.x, point.y])
            p1 = p0 + np.array([delta_x, delta_y])
            l = 1/3*hypot(p1[0] - p0[0], p1[1] - p0[1])

            b0 = p0
            b1 = p0 + l*np.array([cos(point.phi), sin(point.phi)])
            b2 = p1 - l*np.array([cos(point.phi + angle), sin(point.phi + angle)])
            b3 = p1
            
            return from_bezier_points4(b0, b1, b2, b3)
        
        def case_l_a(length, angle):
            def get_circle_point(point, k, s):
                if k != 0:
                    return np.array([
                        point.x + 1/k*(sin(point.phi + k*s) - sin(point.phi)),
                        point.y - 1/k*(cos(point.phi + k*s) - cos(point.phi))
                    ])
                else:
                    return np.array([
                        point.x + s*cos(point.phi),
                        point.y + s*sin(point.phi)
                    ])

            k = angle/length
            p0 = get_circle_point(point, k, 0)
            p1 = get_circle_point(point, k, 1/3*length)
            p2 = get_circle_point(point, k, 2/3*length)
            p3 = get_circle_point(point, k, length)
            
            return from_interpolation(p0, p1, p2, p3)
        
        def case_l_x(length, delta_x):
            if length < delta_x:
               raise ValueError('Impossible constraint, length must be greater than delta_x.')
            
            arg = 2*delta_x/length - cos(point.phi)
            if -1 <= arg <= 1:
                alpha = np.sign(point.phi)*acos(2*delta_x/length - cos(point.phi))
                a = length/2
            else:
                alpha = 0
                a = (length - delta_x)/(1 - cos(point.phi))
                
            p0 = np.array([point.x, point.y])
            p1 = p0 + a*np.array([cos(point.phi), sin(point.phi)])
            p2 = p1 + (length - a)*np.array([cos(alpha), sin(alpha)])

            return from_bezier_points3(p0, p1, p2)

        
        def case_l_y(length, delta_y):
            if length < delta_y:
               raise ValueError('Impossible constraint, length must be greater than delta_y.')
            
            arg = 2*delta_y/length - sin(point.phi)
            if -1 <= arg <= 1:
                alpha = np.sign(pi/2 - abs(point.phi))*acos(2*delta_y/length - sin(point.phi))
                a = length/2
            else:
                alpha = 0
                a = (length - delta_y)/(1 - sin(point.phi))
                
            p0 = np.array([point.x, point.y])
            p1 = p0 + a*np.array([cos(point.phi), sin(point.phi)])
            p2 = p1 + (length - a)*np.array([sin(alpha), cos(alpha)])

            return from_bezier_points3(p0, p1, p2)

        def case_a_x(angle, delta_x):
            den = cos(point.phi) + cos(point.phi + angle/2) + cos(point.phi + angle)
            if den == 0:
                return case_a_x_y(angle, delta_x, 0)
            
            a = delta_x/den
            if a < 0:
                return case_a_x_y(angle, delta_x, 0)
            
            p0 = np.array([point.x, point.y])
            p1 = p0 + a*np.array([cos(point.phi), sin(point.phi)])
            p2 = p1 + a*np.array([cos(point.phi + angle/2), sin(point.phi + angle/2)])
            p3 = p2 + a*np.array([cos(point.phi + angle), sin(point.phi + angle)])
            
            return from_bezier_points4(p0, p1, p2, p3)
        
        def case_a_y(angle, delta_y):
            den = sin(point.phi) + sin(point.phi + angle/2) + sin(point.phi + angle)
            if den == 0:
                return case_a_x_y(angle, 0, delta_y)
            
            a = delta_y/den
            if a < 0:
                return case_a_x_y(angle, 0, delta_y)

            p0 = np.array([point.x, point.y])
            p1 = p0 + a*np.array([cos(point.phi), sin(point.phi)])
            p2 = p1 + a*np.array([cos(point.phi + angle/2), sin(point.phi + angle/2)])
            p3 = p2 + a*np.array([cos(point.phi + angle), sin(point.phi + angle)])
            
            return from_bezier_points4(p0, p1, p2, p3)
        
        def case_x_y(delta_x, delta_y):
            a = atan2(delta_y, delta_x) - point.phi
            return case_a_x_y(a, delta_x, delta_y)

        # Case 1: {length, angle, delta_x, delta_y}
        if (specs.length is not None) and (specs.angle is not None) and (specs.delta_x is not None) and (specs.delta_y is not None):
            return case_l_a_x_y(specs.length, specs.angle, specs.delta_x, specs.delta_y)
        
        # Case 2: {length, angle, delta_x}
        if (specs.length is not None) and (specs.angle is not None) and (specs.delta_x is not None):
            return case_l_a_x(specs.length, specs.angle, specs.delta_x)
        
        # Case 3: {length, angle, delta_y}
        if (specs.length is not None) and (specs.angle is not None) and (specs.delta_y is not None):
            return case_l_a_y(specs.length, specs.angle, specs.delta_y)
        
        # Case 4: {length, delta_x, delta_y}        
        if (specs.length is not None) and (specs.delta_x is not None) and (specs.delta_y is not None):
            return case_l_x_y(specs.length, specs.delta_x, specs.delta_y)
        
        # Case 5: {angle, delta_x, delta_y}
        if (specs.angle is not None) and (specs.delta_x is not None) and (specs.delta_y is not None):
            return case_a_x_y(specs.angle, specs.delta_x, specs.delta_y)

        # Case 6: {length, angle}
        if (specs.length is not None) and (specs.angle is not None):
            return case_l_a(specs.length, specs.angle)

        # Case 7: {length, delta_x}
        if (specs.length is not None) and (specs.delta_x is not None):
            return case_l_x(specs.length, specs.delta_x)

        # Case 8: {length, delta_y}
        if (specs.length is not None) and (specs.delta_y is not None):
            return case_l_y(specs.length, specs.delta_y)

        # Case 9: {angle, delta_x}
        if (specs.angle is not None) and (specs.delta_x is not None):
            return case_a_x(specs.angle, specs.delta_x)

        # Case 10: {angle, delta_y}
        if (specs.angle is not None) and (specs.delta_y is not None):
            return case_a_y(specs.angle, specs.delta_y)
        
        # Case 11: {delta_x, delta_y}
        if((specs.delta_x is not None) and (specs.delta_y is not None)):
            return case_x_y(specs.delta_x, specs.delta_y)

        raise ValueError('Segment not fully specified, at least two constraints are needed.')

    def get_c(self):
        return self.c
    
    def set_c(self, c):
        self.c = c

    # x coordinate
    def x(self, t):
        return self.c[0] + t*(self.c[1] + t*(self.c[2] + t*self.c[3]))
    
    # y coordinate
    def y(self, t):
        return self.c[4] + t*(self.c[5] + t*(self.c[6] + t*self.c[7]))

    # Orientation angle in [-PI, PI]
    def phi(self, t):
        return atan2(self.dydt(t), self.dxdt(t))

    # Total length of the curve
    def length(self):
        return integrate.quad(lambda t: hypot(self.dxdt(t), self.dydt(t)), 0, 1)[0]

    # Total angular change of the curve, not necessarily in [-PI, PI]
    def angle(self):
        return integrate.quad(lambda t: (self.dxdt(t)*self.dydt2(t) - self.dydt(t)*self.dxdt2(t))/(self.dxdt(t)**2 + self.dydt(t)**2), 0, 1)[0]

    # Integral of the squared curvature
    def energy(self):
        #return integrate.quad(lambda t: (self.k(t)**2)*self.dldt(t), 0, 1)[0]
        return integrate.quad(lambda t: (self.dxdt(t)*self.dydt2(t) - self.dydt(t)*self.dxdt2(t))**2/(self.dxdt(t)**2 + self.dydt(t)**2)**2.5, 0, 1)[0]

    # Derivatives t
    
    def dxdt(self, t):
        return self.c[1] + t*(2*self.c[2] + 3*t*self.c[3])

    def dydt(self, t):
        return self.c[5] + t*(2*self.c[6] + 3*t*self.c[7])
    
    def dxdt2(self, t):
        return 2*self.c[2] + 6*self.c[3]*t
    
    def dydt2(self, t):
        return 2*self.c[6] + 6*self.c[7]*t
    
    # Derivatives c

    def grad_x(self, t):
        return np.array([1, t, t**2, t**3, 0, 0, 0, 0])
    
    def grad_y(self, t):
        return np.array([0, 0, 0, 0, 1, t, t**2, t**3])

    def grad_dxdt(self, t):
        return np.array([0, 1, 2*t, 3*t**2, 0, 0, 0, 0])
    
    def grad_dydt(self, t):
        return np.array([0, 0, 0, 0, 0, 1, 2*t, 3*t**2])

    def grad_dxdt2(self, t):
        return np.array([0, 0, 2, 6*t, 0, 0, 0, 0])   
        
    def grad_dydt2(self, t):
        return np.array([0, 0, 0, 0, 0, 0, 2, 6*t])

    def grad_phi(self, t):
        return (self.grad_dydt(t)*self.dxdt(t) - self.grad_dxdt(t)*self.dydt(t))/(self.dxdt(t)**2 + self.dydt(t)**2)

    def grad_length(self):
        def grad_dldt(t):
            return (self.dxdt(t)*self.grad_dxdt(t) + self.dydt(t)*self.grad_dydt(t))/hypot(self.dxdt(t), self.dydt(t))
        return integrate.quad_vec(grad_dldt, 0, 1)[0]
    
    def grad_angle(self):
        return self.grad_phi(1) - self.grad_phi(0)
    
    def grad_energy(self):
        def a(t):
            return self.dxdt(t)*self.dydt2(t) - self.dydt(t)*self.dxdt2(t)
        def b(t):
            return self.dxdt(t)**2 + self.dydt(t)**2
        def grad_a(t):
            return self.dydt2(t)*self.grad_dxdt(t) + self.dxdt(t)*self.grad_dydt2(t) - self.dxdt2(t)*self.grad_dydt(t) - self.dydt(t)*self.grad_dxdt2(t)
        def grad_b(t):
            return 2*self.dxdt(t)*self.grad_dxdt(t) + 2*self.dydt(t)*self.grad_dydt(t)
        def grad_dedt(t):
            return a(t)/b(t)**5*(2*b(t)**(5/2)*grad_a(t) - 5/2*a(t)*b(t)**(3/2)*grad_b(t))
        
        return integrate.quad_vec(grad_dedt, 0, 1)[0]

    # Plotting

    def plot(self):
        x, y = [], []
        for t in np.linspace(0, 1, 1000):
            x.append(self.x(t))
            y.append(self.y(t))
        plt.plot(x, y, color='blue')
        plt.plot([x[ 0]], [y[ 0]], marker='o', color='red')
        plt.plot([x[-1]], [y[-1]], marker='o', color='red')

class Optimization:
    @staticmethod
    def objective(x, grad):
        result = 0
        for index in range(0, int(len(x)/Segment.n_coeffs())):
            i = (index + 0)*Segment.n_coeffs()
            j = (index + 1)*Segment.n_coeffs()
            
            segment = Segment(x[i:j])
            if grad.size > 0:
                grad[i:j] = segment.grad_energy()

            result += segment.energy()
        
        print(result)
        return result

    @staticmethod
    def constraint_phi0(x, grad, i, j, value):
        segment = Segment(x[i:j])
        if grad.size > 0:
            grad[:] = 0
            grad[i:j] = segment.grad_phi(0)
        
        return segment.phi(0) - value
    
    @staticmethod
    def constraint_x0(x, grad, i, j, value):
        segment = Segment(x[i:j])
        if grad.size > 0:
            grad[:] = 0
            grad[i:j] = segment.grad_x(0)
                
        return segment.x(0) - value

    @staticmethod
    def constraint_y0(x, grad, i, j, value):
        segment = Segment(x[i:j])
        if grad.size > 0:
            grad[:] = 0
            grad[i:j] = segment.grad_y(0)
                
        return segment.y(0) - value

    @staticmethod
    def constraint_length(x, grad, i, j, value):
        segment = Segment(x[i:j])
        if grad.size > 0:
            grad[:] = 0
            grad[i:j] = segment.grad_length()
                
        return segment.length() - value
    
    @staticmethod
    def constraint_angle(x, grad, i, j, value):
        segment = Segment(x[i:j])
        if grad.size > 0:
            grad[:] = 0
            grad[i:j] = segment.grad_angle()
                
        return segment.angle() - value
    
    @staticmethod
    def constraint_delta_x(x, grad, i, j, value):
        segment = Segment(x[i:j])
        if grad.size > 0:
            grad[:] = 0
            grad[i:j] = segment.grad_x(1) - segment.grad_x(0)
                
        return segment.x(1) - segment.x(0) - value
    
    @staticmethod
    def constraint_delta_y(x, grad, i, j, value):
        segment = Segment(x[i:j])
        if grad.size > 0:
            grad[:] = 0
            grad[i:j] = segment.grad_y(1) - segment.grad_y(0)
                
        return segment.y(1) - segment.y(0) - value

    @staticmethod
    def constraint_x(x, grad, i0, j0, i1, j1):
        segment0 = Segment(x[i0:j0])
        segment1 = Segment(x[i1:j1])
        if grad.size > 0:
            grad[:] = 0
            grad[i0:j0] = -segment0.grad_x(1)
            grad[i1:j1] = segment1.grad_x(0)
        
        return segment1.x(0) - segment0.x(1)
    
    @staticmethod
    def constraint_y(x, grad, i0, j0, i1, j1):
        segment0 = Segment(x[i0:j0])
        segment1 = Segment(x[i1:j1])
        if grad.size > 0:
            grad[:] = 0
            grad[i0:j0] = -segment0.grad_y(1)
            grad[i1:j1] = segment1.grad_y(0)
                
        return segment1.y(0) - segment0.y(1)

    @staticmethod
    def constraint_dxdt(x, grad, i0, j0, i1, j1):
        segment0 = Segment(x[i0:j0])
        segment1 = Segment(x[i1:j1])
        if grad.size > 0:
            grad[:] = 0
            grad[i0:j0] = -segment0.grad_dxdt(1)
            grad[i1:j1] = segment1.grad_dxdt(0)
                
        return segment1.dxdt(0) - segment0.dxdt(1)
        
    @staticmethod
    def constraint_dydt(x, grad, i0, j0, i1, j1):
        segment0 = Segment(x[i0:j0])
        segment1 = Segment(x[i1:j1])
        if grad.size > 0:
            grad[:] = 0
            grad[i0:j0] = -segment0.grad_dydt(1)
            grad[i1:j1] = segment1.grad_dydt(0)
        
        return segment1.dydt(0) - segment0.dydt(1)
    
    @staticmethod
    def constraint_dxdt2(x, grad, i0, j0, i1, j1):
        segment0 = Segment(x[i0:j0])
        segment1 = Segment(x[i1:j1])
        if grad.size > 0:
            grad[:] = 0
            grad[i0:j0] = -segment0.grad_dxdt2(1)
            grad[i1:j1] = segment1.grad_dxdt2(0)
        
        return segment1.dxdt2(0) - segment0.dxdt2(1)
    
    @staticmethod
    def constraint_dydt2(x, grad, i0, j0, i1, j1):
        segment0 = Segment(x[i0:j0])
        segment1 = Segment(x[i1:j1])
        if grad.size > 0:
            grad[:] = 0
            grad[i0:j0] = -segment0.grad_dydt2(1)
            grad[i1:j1] = segment1.grad_dydt2(0)

        return segment1.dydt2(0) - segment0.dydt2(1)

class ProfileCurve:
    def __init__(self, segment_specs):
        startpoint = Point(x=0.0, y=0.0, phi=0.0)
        coeffs = self.optimize_local(startpoint, segment_specs)
        #coeffs = self.optimize_global(coeffs, startpoint, segment_specs)

        self.segments = []
        for c in coeffs:
            self.segments.append(Segment(c))
        
        self.transform = self.calculate_transform()

    # Determines segment curve that starts at point and fulfills the constraints given by spec
    @staticmethod
    def optimize_segment(point, specs):        
        c0 = Segment.estimate_coeffs(point, specs)

        opt = nlopt.opt(NLOPT_ALGORITHM, len(c0))
        opt.set_ftol_rel(NLOPT_FTOL_REL);
        opt.set_ftol_abs(NLOPT_FTOL_ABS);
        opt.set_maxeval(NLOPT_MAXEVAL);


        print()
        print("Optimize segment")
        print()
#        grad = np.array([0.0]*len(c0))
#        f = Optimization.objective(c0, grad)
#        print("Initial f: {}".format(f))
#        print("Initial g: {}".format(grad))
#        print()

        # Objective function
        opt.set_min_objective(Optimization.objective)
        
        # Starting point constraints
        opt.add_equality_constraint(partial(Optimization.constraint_phi0, i=0, j=Segment.n_coeffs(), value=point.phi), NLOPT_CTOL_ABS)
        opt.add_equality_constraint(partial(Optimization.constraint_x0, i=0, j=Segment.n_coeffs(), value=point.x), NLOPT_CTOL_ABS)
        opt.add_equality_constraint(partial(Optimization.constraint_y0, i=0, j=Segment.n_coeffs(), value=point.y), NLOPT_CTOL_ABS)
        
        # Input constraints
        if(specs.length is not None):
            opt.add_equality_constraint(partial(Optimization.constraint_length, i=0, j=Segment.n_coeffs(), value=specs.length), NLOPT_CTOL_ABS)
        if(specs.angle is not None):
            opt.add_equality_constraint(partial(Optimization.constraint_angle, i=0, j=Segment.n_coeffs(), value=specs.angle), NLOPT_CTOL_ABS)
        if(specs.delta_x is not None):
            opt.add_equality_constraint(partial(Optimization.constraint_delta_x, i=0, j=Segment.n_coeffs(), value=specs.delta_x), NLOPT_CTOL_ABS)
        if(specs.delta_y is not None):
            opt.add_equality_constraint(partial(Optimization.constraint_delta_y, i=0, j=Segment.n_coeffs(), value=specs.delta_y), NLOPT_CTOL_ABS)

        c_min = opt.optimize(c0)       
        return c_min
    
    @staticmethod
    def optimize_local(startpoint, segment_specs):
        coeffs = []
        
        point = startpoint
        for specs in segment_specs:
            coeffs.append(ProfileCurve.optimize_segment(point, specs))
            segment = Segment(coeffs[-1])
            point = Point(x=segment.x(1), y=segment.y(1), phi=segment.phi(1))
            
        return coeffs
    
    @staticmethod
    def optimize_global(coeffs, startpoint, segment_specs):
        x0 = np.concatenate(coeffs)
        opt = nlopt.opt(NLOPT_ALGORITHM, len(x0))
        opt.set_ftol_rel(NLOPT_FTOL_REL);
        opt.set_ftol_abs(NLOPT_FTOL_ABS);
        opt.set_maxeval(NLOPT_MAXEVAL);
        
        # Objective function
        opt.set_min_objective(Optimization.objective)
        
        # Starting point constraints
        opt.add_equality_constraint(partial(Optimization.constraint_phi0, i=0, j=Segment.n_coeffs(), value=startpoint.phi), NLOPT_CTOL_ABS)
        opt.add_equality_constraint(partial(Optimization.constraint_x0, i=0, j=Segment.n_coeffs(), value=startpoint.x), NLOPT_CTOL_ABS)
        opt.add_equality_constraint(partial(Optimization.constraint_y0, i=0, j=Segment.n_coeffs(), value=startpoint.y), NLOPT_CTOL_ABS)
        
        # Input constraints
        for index, specs in enumerate(segment_specs):
            i = (index + 0)*Segment.n_coeffs()
            j = (index + 1)*Segment.n_coeffs()

            if(specs.length is not None):
                opt.add_equality_constraint(partial(Optimization.constraint_length, i=i, j=j, value=specs.length), NLOPT_CTOL_ABS)
            if(specs.angle is not None):
                opt.add_equality_constraint(partial(Optimization.constraint_angle, i=i, j=j, value=specs.angle), NLOPT_CTOL_ABS)
            if(specs.delta_x is not None):
                opt.add_equality_constraint(partial(Optimization.constraint_delta_x, i=i, j=j, value=specs.delta_x), NLOPT_CTOL_ABS)
            if(specs.delta_y is not None):
                opt.add_equality_constraint(partial(Optimization.constraint_delta_y, i=i, j=j, value=specs.delta_y), NLOPT_CTOL_ABS)
        
        # Segment interconnection
        for index in range(0, len(segment_specs) - 1):
            i0 = (index + 0)*Segment.n_coeffs()
            j0 = (index + 1)*Segment.n_coeffs()
            i1 = (index + 1)*Segment.n_coeffs()
            j1 = (index + 2)*Segment.n_coeffs()
            
            opt.add_equality_constraint(partial(Optimization.constraint_x, i0=i0, j0=j0, i1=i1, j1=j1), NLOPT_CTOL_ABS)
            opt.add_equality_constraint(partial(Optimization.constraint_y, i0=i0, j0=j0, i1=i1, j1=j1), NLOPT_CTOL_ABS) 
            opt.add_equality_constraint(partial(Optimization.constraint_dxdt, i0=i0, j0=j0, i1=i1, j1=j1), NLOPT_CTOL_ABS)
            opt.add_equality_constraint(partial(Optimization.constraint_dydt, i0=i0, j0=j0, i1=i1, j1=j1), NLOPT_CTOL_ABS)
            opt.add_equality_constraint(partial(Optimization.constraint_dxdt2, i0=i0, j0=j0, i1=i1, j1=j1), NLOPT_CTOL_ABS)
            opt.add_equality_constraint(partial(Optimization.constraint_dydt2, i0=i0, j0=j0, i1=i1, j1=j1), NLOPT_CTOL_ABS)
        
        x_min = opt.optimize(x0)
        return np.split(x_min, len(segment_specs))
    
    # Returns a function t(l) that mapps arc length l to the curve parameter t in [0, 1]
    def calculate_transform(self):
        def dldt(t):
            n = len(self.segments)
            i = min(floor(n*t), n-1)
            return n*hypot(self.segments[i].dxdt(n*t - i), self.segments[i].dydt(n*t - i))
        
        k = 1000    # Magic number
        t = np.linspace(0, 1, k)
        l = [0]*k

        # Integration by Simpson-method.
        for i in range(1, len(t)):
            l[i] = l[i-1] + (t[i] - t[i-1])/6.0*(dldt(t[i-1]) + 4.0*dldt((t[i] + t[i-1])/2.0) + dldt(t[i]))
               
        return CubicSpline(l, t)
    
    def length(self):
        return self.transform.x[-1]
    
    def x(self, s):
        t = self.transform(s)
        n = len(self.segments)
        i = min(floor(n*t), n-1)
        return self.segments[i].x(n*t - i)
    
    def y(self, s):
        t = self.transform(s)
        n = len(self.segments)
        i = min(floor(n*t), n-1)
        return self.segments[i].y(n*t - i)
    
    def phi(self, s):
        t = self.transform(s)
        n = len(self.segments)
        i = min(floor(n*t), n-1)
        return self.segments[i].phi(n*t - i)
    
    def plot(self):
        x, y = [], []
        for l in np.linspace(0, self.length(), 1000):
            x.append(self.x(l))
            y.append(self.y(l))
        plt.plot(x, y, color='blue')
        
        plt.plot([self.segments[0].x(0)], [self.segments[0].y(0)], marker='o', color='red')
        for segment in self.segments:
            plt.plot([segment.x(1)], [segment.y(1)], marker='o', color='red')

## Test {dl, dphi}
#curve = ProfileCurve([
#    SegmentSpecs(length=1.0, angle=-0.2),
#    SegmentSpecs(length=1.0, angle=0.2),
#    SegmentSpecs(length=0.4, angle=0.9),
#    SegmentSpecs(length=0.2, angle=0.5),
#])

curve = ProfileCurve([
    SegmentSpecs(delta_x=0.1, delta_y=0.01),
    SegmentSpecs(delta_x=0.1, delta_y=0.01),
    SegmentSpecs(delta_x=0.1, delta_y=0.01),
    SegmentSpecs(delta_x=0.1, delta_y=0.01),
    SegmentSpecs(delta_x=0.1, delta_y=0.01),
    SegmentSpecs(delta_x=0.1, delta_y=0.1),
])

# Create plot

plt.title('Profile Curve')
plt.xlabel('x')
plt.ylabel('y')
curve.plot()
plt.grid()
plt.show()