import matplotlib.pyplot as plt
from scipy.optimize import minimize, NonlinearConstraint
from scipy import integrate
from numpy import linspace
from math import sin, cos, hypot, atan2, pi

class Point:
    def __init__(self, s, phi, x, y):
        self.s = s
        self.phi = phi
        self.x = x
        self.y = y
        
    def plot(self):
        dx = 0.1*cos(self.phi)
        dy = 0.1*sin(self.phi)    
        plt.plot([self.x], [self.y], marker='o', color='red')
        plt.plot([self.x, self.x + dx], [self.y, self.y + dy], color='red')

class Segment:
    def __init__(self, p0, c):
        self.p0 = p0
        self.c = c
    
    def k(self, s):
        return self.c[0] + self.c[1]*s + self.c[2]*s**2
    
    def phi(self, s):
        return self.p0.phi + self.c[0]*s + 1/2*self.c[1]*s**2 + 1/3*self.c[2]*s**3

    def x(self, s):
        return self.p0.x + integrate.quad(lambda t: cos(self.phi(t)), 0, s)[0]
    
    def y(self, s):
        return self.p0.y + integrate.quad(lambda t: sin(self.phi(t)), 0, s)[0]

    def I(self):
        #return integrate.quad(lambda t: self.k(t)**2, 0, self.c[3])[0]
        return self.c[0]**2*self.c[3] + self.c[0]*self.c[1]*self.c[3]**2 + (2/3*self.c[0]*self.c[2] + 1/3*self.c[1]**2)*self.c[3]**3 + 1/2*self.c[1]*self.c[2]*self.c[3]**4 + 1/5*self.c[2]**2*self.c[3]**5
    
    def plot(self):
        x, y = [], []
        for s in linspace(0, self.c[3], 100):
            x.append(self.x(s))
            y.append(self.y(s))
        plt.plot(x, y, color='blue')
        
    def get_c_start(p0, p1):
        # {s, phi, x, y}
        if ((p1.s is not None) and (p1.phi is not None) and (p1.x is not None) and (p1.y is not None)):
            return Segment.get_c_start_x_y(p0, p1.x, p1.y)
        
        # {s, phi, x}
        if ((p1.s is not None) and (p1.phi is not None) and (p1.x is not None)):
            return Segment.get_c_start_phi_x(p0, p1.phi, p1.x)
        
        # {s, phi, y}
        if ((p1.s is not None) and (p1.phi is not None) and (p1.y is not None)):
            return Segment.get_c_start_phi_y(p0, p1.phi, p1.y)
        
         # {s, x, y}
        if ((p1.s is not None) and (p1.x is not None) and (p1.y is not None)):
            return Segment.get_c_start_x_y(p0, p1.x, p1.y)
        
         # {phi, x, y}
        if ((p1.phi is not None) and (p1.x is not None) and (p1.y is not None)):
            return Segment.get_c_start_x_y(p0, p1.x, p1.y)
        
        # {s, phi}
        if (p1.s is not None) and (p1.phi is not None):
            return Segment.get_c_start_s_phi(p0, p1.s, p1.phi)
        
        # {s, x}
        if (p1.s is not None) and (p1.x is not None):
            return Segment.get_c_start_s_x(p0, p1.s, p1.x)
        
        # {s, y}
        if (p1.s is not None) and (p1.y is not None):
            return Segment.get_c_start_s_y(p0, p1.s, p1.y)
        
        # {phi, x}
        if (p1.phi is not None) and (p1.x is not None):
            return Segment.get_c_start_phi_x(p0, p1.phi, p1.x)
        
        # {phi, y}
        if (p1.phi is not None) and (p1.y is not None):
            return Segment.get_c_start_phi_y(p0, p1.phi, p1.y)
        
        # {x, y}
        if (p1.x is not None) and (p1.y is not None):
            return Segment.get_c_start_x_y(p0, p1.x, p1.y)
            
        raise Exception('Error in point {}. At least two entries have to be specified.'.format(i+1))
        
    def get_c_start_x_y(p0, x1, y1):
        l = hypot(x1 - p0.x, y1 - p0.y)
        a = 2*(atan2(y1 - p0.y, x1 - p0.x) - p0.phi)
        c0 = 2/l*sin(a/2)
        c3 = a/c0
        return [c0, 0, 0, c3]
        
    def get_c_start_s_phi(p0, s1, phi1):
        c3 = s1 - p0.s
        c0 = (phi1 - p0.phi)/c3
        return [c0, 0, 0, c3]

    def get_c_start_s_x(p0, s1, x1):
        if x1 > p0.x:
            return Segment.get_c_start_phi_x(p0, 0, x1)
        elif x1 < p0.x:
            return Segment.get_c_start_phi_x(p0, pi, x1)
        else:
            c3 = s1 - p0.s
            if p0.phi >= 0:
                c0 = (pi - 2*p0.phi)/c3
            else:
                c0 = (-pi - 2*p0.phi)/c3
            return [c0, 0, 0, c3]

    def get_c_start_s_y(p0, s1, y1):
        if y1 > p0.y:
            return Segment.get_c_start_phi_y(p0, pi/2, y1)
        elif y1 < p0.y:
            return Segment.get_c_start_phi_y(p0, -pi/2, y1)
        else:
            c3 = s1 - p0.s
            if -pi/2 < p0.phi < pi/2:
                c0 = -2*p0.phi/c3
            else:
                c0 = (2*pi - 2*p0.phi)/c3
            return [c0, 0, 0, c3]
            
    def get_c_start_phi_x(p0, phi1, x1):
        if x1 == p0.x:
            raise Exception('Error in point {}. Segment not uniquely determined. X values must be different.'.format(i+1))
        if phi1 == p0.phi:
            return Segment.get_c_start_phi_x(p0, phi1 + 0.1, x1)

        c0 = (sin(phi1) - sin(p0.phi))/(x1 - p0.x)
        c3 = (phi1 - p0.phi)/c0
        if c3 < 0:
            c3 = 2*pi/abs(c0) - abs(c3)
        return [c0, 0, 0, c3]
         
    def get_c_start_phi_y(p0, phi1, y1):
        if phi1 == p0.phi:
            return Segment.get_c_start_phi_y(p0, phi1 + 0.1, y1)
        
        if y1 == p0.y:
            raise Exception('Error in point {}. Segment not uniquely determined. Y values must be different.'.format(i+1))
        c0 = -(cos(phi1) - cos(p0.phi))/(y1 - p0.y)
        c3 = (phi1 - p0.phi)/c0
        if c3 < 0:
            c3 = 2*pi/abs(c0) - abs(c3)
        return [c0, 0, 0, c3]

class ProfileCurve:
    def __init__(self, points):
        # First point: Set unspecified data to zero
        if points[0].s is None:
            points[0].s = 0
        if points[0].phi is None:
            points[0].phi = 0
        if points[0].x is None:
            points[0].x = 0
        if points[0].y is None:
            points[0].y = 0
        
        self.points = points
        self.segments = []
        
        for i in range(len(points)-1):
            p0 = points[i]
            p1 = points[i+1]
            
            def f(c):
                curve = Segment(p0, c)
                return curve.I()

            def g(c):
                curve = Segment(p0, c)
                result = []
                if p1.s is not None:
                    result.append(curve.c[3] - (p1.s - p0.s))
                if p1.phi is not None:
                    result.append(curve.phi(curve.c[3]) - p1.phi)
                if p1.x is not None:
                    result.append(curve.x(curve.c[3]) - p1.x)
                if p1.y is not None:
                    result.append(curve.y(curve.c[3]) - p1.y)
                return result


            c_start = Segment.get_c_start(p0, p1)
            result = minimize(f, c_start, constraints=NonlinearConstraint(g, 0, 0), method='SLSQP')
            segment = Segment(p0, result.x)
            
            self.segments.append(segment)
            if p1.s is None:
                p1.s = p0.s + segment.c[3]
            if p1.phi is None:
                p1.phi = segment.phi(segment.c[3])
            if p1.x is None:
                p1.x = segment.x(segment.c[3])
            if p1.y is None:
                p1.y = segment.y(segment.c[3])
    

   
    def plot(self):
        for s in self.segments:
            s.plot()
        for p in self.points:
            p.plot()

# Input parameters

points = [
    Point(s=0, phi=0.0, x=0, y=0),
    Point(s=1, phi=0.5, x=None, y=None),
    Point(s=1.5, phi=None, x=1.3, y=None),
   Point(s=None, phi=None, x=1.5, y=0.5)
]

curve = ProfileCurve(points)

# Create plot

curve.plot()
plt.title('Profile Curve')
plt.xlabel('x')
plt.ylabel('y')
plt.grid()
plt.show()
