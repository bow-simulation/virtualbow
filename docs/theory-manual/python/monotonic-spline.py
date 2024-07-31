import matplotlib.pyplot as plt
from scipy.optimize import minimize, NonlinearConstraint
from scipy.misc import derivative
from scipy import integrate
from numpy import linspace, inf, sign
from math import sin, cos, hypot, atan2, pi

class NaturalCubicSpline:
    def __init__(self, x, y):
        self.x = x
        self.y = y
    
        a = []
        b = []
        c = []

        for i in range(0, len(x)-1):
            a.append(0.0);
            b.append(0.0);
            c.append((y[i+1] - y[i])/(x[i+1] - x[i]));

        def get_parameters(a, b, c):
            return a + b + c
        
        def get_coefficients(p):
            n = int(round(len(p)/3))
            return (p[0:n], p[n:2*n], p[2*n:3*n])
        
        def f(p):
            (a, b, c) = get_coefficients(p)
            result = 0
            for i in range(0, len(a)):
                dx = x[i+1] - x[i]
                result += 12*a[i]**2*dx**3 + 12*a[i]*b[i]*dx**2 + 4*b[i]**2*dx
      
            return result

        def g(p):
            (a, b, c) = get_coefficients(p)
            result = []
            
            # Interpolation
            for i in range(0, len(a)):
                dx = x[i+1] - x[i]
                dy = y[i+1] - y[i]
                result.append(a[i]*dx**3 + b[i]*dx**2 + c[i]*dx - dy)
                
            # Continuity of first and second derivative
            for i in range(0, len(a)-1):
                dx = x[i+1] - x[i]
                dy = y[i+1] - y[i]                
                result.append(3*a[i]*dx**2 + 2*b[i]*dx + c[i] - c[i+1])
                result.append(6*a[i]*dx + 2*b[i] - 2*b[i+1])
            
            return result

        p_start = get_parameters(a, b, c)
        result = minimize(f, p_start, constraints=NonlinearConstraint(g, 0, 0), method='SLSQP')
        (self.a, self.b, self.c) = get_coefficients(result.x)
            
    def value(self, arg):
        index = 0
        while not (self.x[index] <= arg <= self.x[index + 1]):
            index += 1
        
        h = arg - self.x[index];
        return ((self.a[index]*h + self.b[index])*h + self.c[index])*h + self.y[index];
    
    def arg_min(self):
        return self.x[0]
    
    def arg_max(self):
        return self.x[-1]
        
    def plot(self):
        x = []
        y = []
        for xi in linspace(self.arg_min(), self.arg_max(), 100):
            x.append(xi)
            y.append(self.value(xi))
            
        plt.plot(x, y, linestyle='--', color='green')
        plt.plot([self.x], [self.y], marker='o', color='red')
        
class MonotonicCubicSpline:
    def __init__(self, x, y):
        self.x = x
        self.y = y

        # Linearized energy
        def f_le(m):
            result = 0
            for i in range(0, len(m)-1):
                dx = x[i+1] - x[i]
                dy = y[i+1] - y[i]
                result += (12*dy**2 - 12*(m[i+1] + m[i])*dx*dy + 4*(m[i+1]**2 + m[i]*m[i+1] + m[i]**2)*dx**2)/dx**3
            
            return result
    
        # Second derivative discontinuity energy
        def f_de(m):
            result = 0
            for i in range(1, len(m)-2):
                dx1 = x[i+1] - x[i]
                dx0 = x[i] - x[i-1]
                result += ((-3*y[i] + 3*y[i+1] - 2*dx1*m[i] - dx1*m[i+1])/dx1**2 - (3*m[i-1] - 3*y[i] + dx0*m[i-1] + 2*dx0*m[i]))**2
            
            return result

        # Monotonicity constraints (inequality)
        def g_ie(m):
            result = []
            for i in range(0, len(m)-1):
                ms = (y[i+1] - y[i])/(x[i+1] - x[i])
                if ms != 0:
                    result.append(-m[i]*sign(ms))
                    result.append(-m[i+1]*sign(ms))
                    result.append((m[i] - m[i+1])*sign(ms) - 3*abs(ms))
                    result.append((m[i+1] - m[i])*sign(ms) - 3*abs(ms))
                    result.append((2*m[i] + m[i+1])*sign(ms) - 9*abs(ms))
                    result.append((m[i] + 2*m[i+1])*sign(ms) - 9*abs(ms))
            
            return result

        # Monotonicity constraints (equality)
        def g_eq(m):
            result = []
            for i in range(0, len(m)-1):
                ms = (y[i+1] - y[i])/(x[i+1] - x[i])
                if ms == 0:
                    result.append(m[i])
                    result.append(m[i+1])

            return result

        # Determine needed types of constraints (equality, inequality)
        constraints = []
        for i in range(0, len(y)-1):
            if y[i] != y[i+1]:
                constraints.append(NonlinearConstraint(g_ie, -inf, 0))
                break;
        for i in range(0, len(y)-1):
            if y[i] == y[i+1]:
                constraints.append(NonlinearConstraint(g_eq, 0, 0))
                break
              
        m_start = [0]*len(x)
        result = minimize(f_le, m_start, constraints=constraints, method='SLSQP')
        self.m = result.x

        print('Initial: {}'.format(f_le(m_start)))
        print('Minimum: {}'.format(f_le(result.x)))
    
    def h00(self, t):
        return 2*t**3 - 3*t**2 + 1

    def h10(self, t):
        return t**3 - 2*t**2 + t
    
    def h01(self, t):
        return -2*t**3 + 3*t**2
    
    def h11(self, t):
        return t**3 - t**2
        
    def value(self, arg):
        index = 0
        while not (self.x[index] <= arg <= self.x[index + 1]):
            index += 1
        
        dx = self.x[index + 1] - self.x[index]
        u = (arg - self.x[index])/dx
        
        return self.h00(u)*self.y[index] + self.h10(u)*self.m[index]*dx + self.h01(u)*self.y[index + 1] + self.h11(u)*self.m[index + 1]*dx
    
    def arg_min(self):
        return self.x[0]
    
    def arg_max(self):
        return self.x[-1]
        
    def plot(self):
        x = []
        y = []
        for xi in linspace(self.arg_min(), self.arg_max(), 100):
            x.append(xi)
            y.append(self.value(xi))
            
        plt.plot(x, y, color='blue')
        plt.plot([self.x], [self.y], marker='o', color='red')
        
        

# Input parameters

#x = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
#y = [0, 1, 4.8, 6, 8, 13, 14, 15.5, 18, 19, 23, 24.1]

x = [0, 0.25, 1, 2]
y = [0.06, 0.03, 0.02, 0.03]

natural = NaturalCubicSpline(x, y)
monotone = MonotonicCubicSpline(x, y)

# Create plot
natural.plot()
monotone.plot()
plt.title('Profile Curve')
plt.xlabel('x')
plt.ylabel('y')
plt.grid()
plt.show()
