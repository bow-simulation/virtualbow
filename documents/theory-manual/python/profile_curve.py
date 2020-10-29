import numpy as np
from math import sin, cos, tan, acos, hypot, atan2, pi, floor
from scipy.optimize import minimize, NonlinearConstraint
from scipy.interpolate import CubicSpline
from scipy import integrate
import matplotlib.pyplot as plt

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
        self.c = c

    @staticmethod
    def get_c_start(point, specs):
        # Returns coefficients from four bezier points
        # https://de.wikipedia.org/wiki/B%C3%A9zierkurve
        def from_bezier_points4(p0, p1, p2, p3):
            a0 = p0
            a1 = -3*p0 + 3*p1
            a2 = 3*p0 - 6*p1 + 3*p2
            a3 = -p0 + 3*p1 - 3*p2 + p3
            
            return [a0[0], a1[0], a2[0], a3[0], a0[1], a1[1], a2[1], a3[1]]
        
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
            a = atan2(delta_y, delta_x)
            return case_a_x_y(2*a, delta_x, delta_y)

        # Case 1: {length, angle, delta_x, delta_y}
        if((specs.length is not None) and (specs.angle is not None) and (specs.delta_x is not None) and (specs.delta_y is not None)):
            return case_l_a_x_y(specs.length, specs.angle, specs.delta_x, specs.delta_y)
        
        # Case 2: {length, angle, delta_x}
        if((specs.length is not None) and (specs.angle is not None) and (specs.delta_x is not None)):
            return case_l_a_x(specs.length, specs.angle, specs.delta_x)
        
        # Case 3: {length, angle, delta_y}
        if((specs.length is not None) and (specs.angle is not None) and (specs.delta_y is not None)):
            return case_l_a_y(specs.length, specs.angle, specs.delta_y)
        
        # Case 4: {length, delta_x, delta_y}        
        if((specs.length is not None) and (specs.delta_x is not None) and (specs.delta_y is not None)):
            return case_l_x_y(specs.length, specs.delta_x, specs.delta_y)
        
        # Case 5: {angle, delta_x, delta_y}
        if((specs.angle is not None) and (specs.delta_x is not None) and (specs.delta_y is not None)):
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

    # Derivatives (private)
    
    def dxdt(self, t):
        return self.c[1] + t*(2*self.c[2] + 3*t*self.c[3])
        
    def dydt(self, t):
        return self.c[5] + t*(2*self.c[6] + 3*t*self.c[7])

    def dxdt2(self, t):
        return 2*self.c[2] + 6*self.c[3]*t
        
    def dydt2(self, t):
        return 2*self.c[6] + 6*self.c[7]*t

    def plot(self):
        x, y = [], []
        for t in np.linspace(0, 1, 1000):
            x.append(self.x(t))
            y.append(self.y(t))
        plt.plot(x, y, color='blue')
        plt.plot([x[ 0]], [y[ 0]], marker='o', color='red')
        plt.plot([x[-1]], [y[-1]], marker='o', color='red')

class ProfileCurve:
    def __init__(self, segment_specs):
        self.segments = []
        startpoint = Point(x=0.0, y=0.0, phi=0.0)
        
        point = startpoint
        for specs in segment_specs:
            segment = ProfileCurve.optimize_local(point, specs)
            point = Point(x=segment.x(1), y=segment.y(1), phi=segment.phi(1))
            self.segments.append(segment)
     
        self.optimize_global(startpoint, segment_specs)
        self.calculate_transform()
        
        #print("Function value: {}".format(f(result.x)))
        #print("Min violation: {}".format(min(g(result.x))))
        #print("Max violation: {}".format(max(g(result.x))))
        #print("g(x): {}".format(g(result.x)))

    # Determines segment curve that starts at point and fulfills the constraints given by spec
    @staticmethod
    def optimize_local(point, specs):
        def f(c):
            segment = Segment(c)
            return segment.energy()
                
        def g(c):
            segment = Segment(c)
            result = []
                
            # Constraints for starting point
            result.append(segment.phi(0) - point.phi)
            result.append(segment.x(0) - point.x)
            result.append(segment.y(0) - point.y)
                
            # Constraints for segment properties
            if specs.length is not None:
                result.append(segment.length() - specs.length)
            if specs.angle is not None:
                result.append(segment.angle() - specs.angle)
            if specs.delta_x is not None:
                result.append(segment.x(1) - segment.x(0) - specs.delta_x)
            if specs.delta_y is not None:
                result.append(segment.y(1) - segment.y(0) - specs.delta_y)
                    
            return result;
        
        c_start = Segment.get_c_start(point, specs)
        result = minimize(f, c_start, constraints=NonlinearConstraint(g, 0, 0), method='SLSQP')
        return Segment(result.x)
    
    def optimize_global(self, point, segment_specs):
        # Mapping of the optimization variables
        def get_x():
            x = []
            for segment in self.segments:
                x.extend(segment.get_c())
            return x
        
        def set_x(x):
            for i in range(len(self.segments)):
                n = len(self.segments[i].get_c())
                self.segments[i].set_c(x[n*i : n*i+n])

        # Objective function: Sum of all segment energies
        def f(x):
            set_x(x)
            return sum(segment.energy() for segment in self.segments)
       
        # Constraint function
        # Given by the specified curve points
        def g(x):
            set_x(x)
            result = []

            # Constraints for segment interconnection
            # Todo: Make dependent on initial point
            result.append(self.segments[0].x(0) - point.x)
            result.append(self.segments[0].y(0) - point.y)
            result.append(self.segments[0].phi(0) - point.phi)
            
            for i in range(len(self.segments) - 1):
                segment0 = self.segments[i]
                segment1 = self.segments[i+1]

                # Position          
                result.append(segment1.x(0) - segment0.x(1))
                result.append(segment1.y(0) - segment0.y(1))
                
                # First derivative
                result.append(segment1.dxdt(0) - segment0.dxdt(1))
                result.append(segment1.dydt(0) - segment0.dydt(1))
                
                # Second derivative
                result.append(segment1.dxdt2(0) - segment0.dxdt2(1))
                result.append(segment1.dydt2(0) - segment0.dydt2(1))                
                
            # Constraints for segment properties
            for i in range(len(self.segments)):
                segment = self.segments[i]
                specs = segment_specs[i]
                
                if specs.length is not None:
                    result.append(segment.length() - specs.length)
                if specs.angle is not None:
                    result.append(segment.angle() - specs.angle)
                if specs.delta_x is not None:
                    result.append(segment.x(1) - segment.x(0) - specs.delta_x)
                if specs.delta_y is not None:
                    result.append(segment.y(1) - segment.y(0) - specs.delta_y)
            
            return result
             
        result = minimize(f, get_x(), constraints=NonlinearConstraint(g, 0, 0), method='SLSQP')
        set_x(result.x)
    
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
               
        self.transform = CubicSpline(l, t)
    
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
            
# Test {dl, dphi}
curve = ProfileCurve([
    SegmentSpecs(length=1.0, angle=-0.2),
    SegmentSpecs(length=1.0, angle=0.2),
    SegmentSpecs(length=0.4, angle=0.9),
    SegmentSpecs(length=0.2, angle=0.5),
])


#print(curve.segments[1].length())

# Create plot

plt.title('Profile Curve')
plt.xlabel('x')
plt.ylabel('y')
curve.plot()
plt.grid()
plt.show()

## https://de.wikipedia.org/wiki/Kubisch_Hermitescher_Spline
#def from_hermite(p0, p1, m0, m1):
#    a0 = p0
#    a1 = m0
#    a2 = -3*p0 + 3*p1 - 2*m0 - m1
#    a3 = 2*p0 - 2*p1 + m0 + m1
#
#    return [a0[0], a1[0], a2[0], a3[0], a0[1], a1[1], a2[1], a3[1]]


# Attempt to calculate coefficients from three points and two angles - doesn't seem to work
#def from_points_and_angles(p0, p1, p2, phi0, phi2):
#    A = np.array([
#        [ 1.0,        0.0,            0.0,            0.0,     0.0,     0.0,     0.0,     0.0 ],
#        [ 0.0,        0.0,            0.0,            0.0,     1.0,     0.0,     0.0,     0.0 ],
#        [ 1.0,    1.0/2.0,        1.0/4.0,        1.0/8.0,     0.0,     0.0,     0.0,     0.0 ],
#        [ 0.0,        0.0,            0.0,            0.0,     1.0, 1.0/2.0, 1.0/4.0, 1.0/8.0 ],
#        [ 1.0,        1.0,            1.0,            1.0,     0.0,     0.0,     0.0,     0.0 ],
#        [ 0.0,        0.0,            0.0,            0.0,     1.0,     1.0,     1.0,     1.0 ],
#        [ 0.0, -tan(phi0),            0.0,            0.0,     0.0,     1.0,     0.0,     0.0 ],
#        [ 0.0, -tan(phi2), -2.0*tan(phi2), -3.0*tan(phi2),     0.0,     1.0,     2.0,     3.0 ]
#    ]);
#
#    p = np.concatenate([p0, p1, p2, [0, 0]])
#    c = np.linalg.solve(A, p)
#    return c.tolist()
#
#p0 = [0, 0]
#p1 = [1, 0]
#p2 = [2, 0]
#
#phi0 = 0.5
#phi2 = 0.5
#
#c = from_points_and_angles(p0, p1, p2, phi0, phi2)
#curve = Segment(c)
