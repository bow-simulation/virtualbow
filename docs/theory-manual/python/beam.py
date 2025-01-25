import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from scipy.integrate import quad
from math import pi, sqrt

class ContinuousBeam:
    def __init__(self, rA, EI, l, w0, v0, n_modes):
        def kappa(i):
            if i == 1:
                return 1.8751/l
            if i == 2:
                return 4.6941/l
            if i == 3:
                return 7.8548/l
            if i == 4:
                return 10.996/l
            if i >= 5:
                return (2*i - 1)/l*pi/2

        self.l     = l
        self.I     = np.arange(1, n_modes + 1)
        self.kappa = np.vectorize(kappa)(self.I)
        self.omega = self.kappa**2*sqrt(EI/rA)

        fw = np.vectorize(lambda i: quad(lambda x: w0(x)*self.W(x)[i-1], 0, l)[0])
        fv = np.vectorize(lambda i: quad(lambda x: v0(x)*self.W(x)[i-1], 0, l)[0])

        self.A     = 1/l*fw(self.I)
        self.B     = 1/(self.omega*l)*fv(self.I)

    def W(self, x):
        gamma = (np.cos(self.kappa*self.l) + np.cosh(self.kappa*self.l))/(np.sin(self.kappa*self.l) + np.sinh(self.kappa*self.l))
        #return np.cos(self.kappa*x) - np.cosh(self.kappa*x) - gamma*(np.sin(self.kappa*x) - np.sinh(self.kappa*x))
        return np.cos(self.kappa*x) - gamma*np.sin(self.kappa*x) - np.cosh(self.kappa*x)*(1 - gamma*np.tanh(self.kappa*x))

    def H(self, t):
        return self.A*np.cos(self.omega*t) + self.B*np.sin(self.omega*t)

    def w(self, x, t):
        return np.vectorize(lambda x: np.sum(self.W(x)*self.H(t)))(x)

rA = 1.0
EI = 10.0
l = 0.9
r = 5.0

n_modes = 10
n_space = 100
n_time  = 100

def w0(x):
    #return 0.1*x**2*(3*l - x)
    return r - sqrt(r**2 - x**2)

def v0(x):
    return 0

beam = ContinuousBeam(rA, EI, l, w0, v0, n_modes)

fig, ax = plt.subplots()
ax.set_xlim([0, l])
ax.set_ylim([-l/2, l/2])

x_eval = np.linspace(0, l, n_space)
line, = ax.plot(x_eval, beam.w(x_eval, 0), linewidth=5)

def animate(i):
    line.set_ydata(beam.w(x_eval, i/n_time))
    return line,

ani = animation.FuncAnimation(fig, animate, interval=n_time, blit=True, save_count=50)
plt.grid()
plt.show()
