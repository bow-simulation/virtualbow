import numpy as np
from math import sin, cos, pi
from scipy.interpolate import CubicHermiteSpline
import matplotlib.pyplot as plt

# Visualizes the rigid body motion of a beam segment

# Initial nodal coordinates (x0, y0, phi0, x1, x1, phi1)
u0 = np.array([1.0, 0.5, -0.5, 2.0, 1.0, 0.5])

# Rigid body motion: translation dx, dy and rotation da
dx = 0.1
dy = 0.0
da = pi/2

A = np.array([
    [1, 0, 0, 0, 0, 0],
    [0, 1, 0, 0, 0, 0],
    [0, 0, 1, 0, 0, 0],
    [1 - cos(da), sin(da), 0, cos(da), -sin(da), 0],
    [-sin(da), 1 - cos(da), 0, sin(da), cos(da), 0],
    [0, 0, 0, 0, 0, 1]
])

b = np.array([dx, dy, da, dx, dy, da])

u1 = A@u0 + b

def coordinates_to_curve(u):
    r0 = [u[0], u[1]]
    d0 = [cos(u[2]), sin(u[2])]
    r1 = [u[3], u[4]]
    d1 = [cos(u[5]), sin(u[5])]
    
    spline = CubicHermiteSpline([0, 1], [r0, r1], [d0, d1])
    points = spline(np.linspace(0, 1, 100))
    
    return points

print(u0)
print(u1)

# Test inverse
A_inv = np.array([
    [1, 0, 0, 0, 0, 0],
    [0, 1, 0, 0, 0, 0],
    [0, 0, 1, 0, 0, 0],
    [1 - cos(da), -sin(da), 0, cos(da), sin(da), 0],
    [sin(da), 1 - cos(da), 0, -sin(da), cos(da), 0],
    [0, 0, 0, 0, 0, 1]
])

u2 = A_inv@(u1 - b)
print(u2 - u0)  # Should be zero

curve0 = coordinates_to_curve(u0)
curve1 = coordinates_to_curve(u1)

plt.plot(curve0[:,0], curve0[:,1])
plt.plot(curve1[:,0], curve1[:,1])
plt.gca().set_aspect('equal')
plt.grid()
plt.show()