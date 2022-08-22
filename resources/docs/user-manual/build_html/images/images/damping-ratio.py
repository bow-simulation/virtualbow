import numpy as np
import matplotlib.pyplot as plt
from math import sqrt

def create_plot(zeta, name):
    omega0 = 1

    T = 2*np.pi/omega0
    n = 4

    t = np.linspace(0, n*T, 1000)
    x = np.exp(-omega0*zeta*t)*np.cos(sqrt(omega0**2 - zeta**2)*t)

    fig, ax = plt.subplots(1, 1, figsize=(8, 4))
    ax.plot(t, x, linewidth=6)
    plt.xlim(0, n*T)
    plt.ylim(-1.05, 1.05)
    plt.xticks([])
    plt.yticks([])
    fig.savefig(name, pad_inches=1)

create_plot(0.0, "damping-ratio-00.svg")
create_plot(0.1, "damping-ratio-01.svg")
create_plot(1.0, "damping-ratio-10.svg")
