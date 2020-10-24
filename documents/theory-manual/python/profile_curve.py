import numpy as np
from math import sin, cos, hypot, atan2, pi, floor
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
# https://en.wikipedia.org/wiki/Differentiable_curve#arc-length_parametrization
# https://en.wikipedia.org/wiki/Tangential_angle
class SegmentCurve:
    def __init__(self, c):
        self.set_c(c)

    @staticmethod
    def get_c_start(point, specs):
        # Normalize angle to a range of [-pi, pi]
        # https://stackoverflow.com/a/24234924
        def normalize_angle(x):
            return x - 2*pi*floor((x + pi)/(2*pi))
        
        # Calculate c for a line starting at the initial point with angle phi and length l
        def linear(phi, l):
            return [point.x, l*cos(phi), 0, 0, point.y, l*sin(phi), 0, 0]
        
        # Given: {dl, dphi}
        if (specs.length is not None) and (specs.angle is not None):
            return linear(point.phi + specs.angle, specs.length)

        # Given: {dl, dx}
        if (specs.length is not None) and (specs.delta_x is not None):
            dl_n = max(specs.length, abs(specs.delta_x))    # If dl < dx, use dx as length
            phi_n = normalize_angle(point.phi)
            alpha = acos(specs.delta_x/dl_n)
            
            if phi_n >= 0:
                if specs.delta_x >= 0:
                    return linear(alpha, specs.length)
                else:
                    return linear(pi - alpha, specs.length)
            else:
                if specs.delta_x >= 0:
                    return linear(-alpha, specs.length)
                else:
                    return linear(alpha - pi, specs.length)
        
        # Given: {dl, dy}
        if (specs.length is not None) and (specs.delta_y is not None):
            dl_n = max(specs.length, abs(specs.delta_y))    # If dl < dy, use dy as length
            phi_n = normalize_angle(point.phi)
            alpha = acos(specs.delta_y/dl_n)
            
            if phi_n >= -pi/2 and phi_n < pi/2:
                if specs.delta_y >= 0:
                    return linear(pi/2 - alpha, specs.length)
                else:
                    return linear(-pi/2 + alpha, specs.length)
            else:
                if specs.delta_y >= 0:
                    return linear(pi/2 + alpha, specs.length)
                else:
                    return linear(-pi/2 - alpha, specs.length)
        
        # Given: {dphi, dx}
        if (specs.angle is not None) and (specs.delta_x is not None):
            if specs.delta_x >= 0:
                return linear(0, specs.delta_x)
            else:
                return linear(pi, -specs.delta_x)
        
        # Given: {dphi, dy}
        if (specs.angle is not None) and (specs.delta_y is not None):
            if specs.delta_y >= 0:
                return linear(pi/2, specs.delta_y)
            else:
                return linear(-pi/2, -specs.delta_y)
            
        # Given: {dx, dy}
        if (specs.delta_x is not None) and (specs.delta_y is not None):
            return linear(atan2(specs.delta_y, specs.delta_x), hypot(specs.delta_x, specs.delta_y))

        # Fallback
        return linear(point.phi, 1)

    def get_c(self):
        return [*self.cx, *self.cy]
    
    def set_c(self, c):
        self.cx = c[:len(c)//2]    # First half or list
        self.cy = c[len(c)//2:]    # Second half of list

    # x coordinate
    def x(self, t):
        return self.cx[0] + t*(self.cx[1] + t*(self.cx[2] + t*self.cx[3]))
    
    # y coordinate
    def y(self, t):
        return self.cy[0] + t*(self.cy[1] + t*(self.cy[2] + t*self.cy[3]))

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
        return self.cx[1] + t*(2*self.cx[2] + 3*t*self.cx[3])
        
    def dydt(self, t):
        return self.cy[1] + t*(2*self.cy[2] + 3*t*self.cy[3])

    def dxdt2(self, t):
        return 2*self.cx[2] + 6*self.cx[3]*t
        
    def dydt2(self, t):
        return 2*self.cy[2] + 6*self.cy[3]*t

    def plot(self):
        x, y = [], []
        for t in np.linspace(0, 1, 100):
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
            result.append(self.segments[0].x(0) - startpoint.x)
            result.append(self.segments[0].y(0) - startpoint.y)
            result.append(self.segments[0].phi(0) - startpoint.phi)
            
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
        
        self.parametrization = self.calculate_parametrization(1000)    # Magic
        
        #print("Function value: {}".format(f(result.x)))
        #print("Min violation: {}".format(min(g(result.x))))
        #print("Max violation: {}".format(max(g(result.x))))
        #print("g(x): {}".format(g(result.x)))

    # Determines segment curve that starts at point and fulfills the constraints given by spec
    @staticmethod
    def optimize_local(point, specs):
        def f(c):
            segment = SegmentCurve(c)
            return segment.energy()
                
        def g(c):
            segment = SegmentCurve(c)
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
        
        c_start = SegmentCurve.get_c_start(point, specs)
        result = minimize(f, c_start, constraints=NonlinearConstraint(g, 0, 0), method='SLSQP')
        return SegmentCurve(result.x)
    
    def length(self):
        return self.parametrization.x[-1]
    
    def x(self, l):
        t = self.parametrization(l)
        n = len(self.segments)
        i = min(floor(n*t), n-1)
        return self.segments[i].x(n*t - i)
    
    def y(self, l):
        t = self.parametrization(l)
        n = len(self.segments)
        i = min(floor(n*t), n-1)
        return self.segments[i].y(n*t - i)
    
    def phi(self, l):
        t = self.parametrization(l)
        n = len(self.segments)
        i = min(floor(n*t), n-1)
        return self.segments[i].phi(n*t - i)

    # Returns a function t(l) that mapps arc length l to the curve parameter t in [0, 1]
    def calculate_parametrization(self, n):
        def dldt(t):
            n = len(self.segments)
            i = min(floor(n*t), n-1)
            return n*hypot(self.segments[i].dxdt(n*t - i), self.segments[i].dydt(n*t - i))
            
        t = np.linspace(0, 1, n)
        l = [0]*n

        # Integration by Simpson-method.
        for i in range(1, len(t)):
            l[i] = l[i-1] + (t[i] - t[i-1])/6.0*(dldt(t[i-1]) + 4.0*dldt((t[i] + t[i-1])/2.0) + dldt(t[i]))
               
        return CubicSpline(l, t)
    
    def plot(self):
        x, y = [], []
        for l in np.linspace(0, self.length(), 100):
            x.append(self.x(l))
            y.append(self.y(l))
        plt.plot(x, y, color='blue')
        
        plt.plot([self.segments[0].x(0)], [self.segments[0].y(0)], marker='o', color='red')
        for segment in self.segments:
            plt.plot([segment.x(1)], [segment.y(1)], marker='o', color='red')
            


#point = Point(x=0.0, y=0.0, phi=0.0)
#specs = SegmentSpecs(delta_x=1.0, delta_y=1.0)
#curve = ProfileCurve.optimize_local(point, specs)

#
#curve = SegmentCurve([0, 1, 0, 0, 0, 0, 10, 0])
#print(curve.energy())

## Test {dl, dphi}
#curve = ProfileCurve([
#    SegmentSpecs(length=1.0, angle=-0.2),
#    SegmentSpecs(length=1.0, angle=0.2),
#    SegmentSpecs(length=0.4, angle=0.9),
#    SegmentSpecs(length=0.2, angle=0.5),
#])

curve = ProfileCurve([
    SegmentSpecs(length=0.8, angle=2.0),
    SegmentSpecs(length=0.2)
])

print(curve.length())

#print(curve.segments[0].k(0.0))
#print(curve.segments[0].k(0.5))
#print(curve.segments[0].energy())
#print(curve.segments[0].length())

# Create plot

plt.title('Profile Curve')
plt.xlabel('x')
plt.ylabel('y')
curve.plot()
plt.grid()
plt.show()


#def get_bezier_points(point, length, angle):
#    
#    l = length/3
#    a = angle/4
#    
#    b0 = [point.x, point.x]
#    b1 = [b0[0] + l*cos(point.phi + 1*a), b0[1] + l*sin(point.phi + 1*a)]
#    b2 = [b1[0] + l*cos(point.phi + 2*a), b1[1] + l*sin(point.phi + 2*a)]
#    b3 = [b2[0] + l*cos(point.phi + 3*a), b2[1] + l*sin(point.phi + 3*a)]
#
#    return (b0, b1, b2, b3)
#
#point = Point(x=0.0, y=0.0, phi=0.0)
#specs = SegmentSpecs(length=None, angle=0.2*pi, delta_x=1.0, delta_y=1.0)
#(b0, b1, b2, b3) = get_bezier_points(point, 3.0, 2*pi)
#plt.plot([b0[0], b1[0], b2[0], b3[0]], [b0[1], b1[1], b2[1], b3[1]], color='red')
#curve = SegmentCurve.from_bezier_points(b0, b1, b2, b3)
#
#    @staticmethod
#    def from_bezier_points(b0, b1, b2, b3):
#        cx0 = b0[0]
#        cy0 = b0[1]
#        
#        cx1 = -3*b0[0] + 3*b1[0]
#        cy1 = -3*b0[1] + 3*b1[1]
#
#        cx2 = 3*b0[0] - 6*b1[0] + 3*b2[0]
#        cy2 = 3*b0[1] - 6*b1[1] + 3*b2[1]
#
#        cx3 = -b0[0] + 3*b1[0] - 3*b2[0] + b3[0]
#        cy3 = -b0[1] + 3*b1[1] - 3*b2[1] + b3[1]
#
#        return SegmentCurve([cx0, cx1, cx2, cx3, cy0, cy1, cy2, cy3])
