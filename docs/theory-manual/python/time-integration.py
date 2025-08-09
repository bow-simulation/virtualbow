import numpy as np
from scipy.integrate import solve_ivp
from scipy.linalg import expm
from scipy.optimize import fsolve
import matplotlib.pyplot as plt
from math import pi, sqrt, tanh

np.random.seed(0)

# System parameters

n = 5

m = 0.1
k = 1e5
d = 0e1

T = 0.1

p_max = 1e6
u_max = 1e0
v_max = 1e3

# Random constant excitation and initial state

P0 = np.ones(n)
u0 = np.ones(n)
v0 = np.ones(n)

# Equation of motion

M = np.diag([m]*n)                                                         # Diagonal mass matrix
K = np.diag([2*k]*n) + np.diag([-k]*(n-1), 1) + np.diag([-k]*(n-1), -1)    # Tridiagonal stiffness matrix
D = np.diag([2*d]*n) + np.diag([-d]*(n-1), 1) + np.diag([-d]*(n-1), -1)    # Tridiagonal damping matrix

def Q(u):
    return K@u

def Kt(u):
    return K

def P(t):
    return P0

def simulate_analytical(t_end, n_out):
    # Linear system matrix
    A = np.block([[np.zeros((n, n)), np.identity(n)], [-np.linalg.solve(M, K), -np.linalg.solve(M, D)]])
    
    # Initial conditions
    x0 = np.concatenate((u0, v0))
    
    # Compute analytical solution with matrix exponential function
    t = np.linspace(0, t_end, n_out)
    X = []

    for ti in t:
        b = np.block([np.zeros(n), np.linalg.solve(M, P0)])
        phi = expm(A*ti)
        X.append(phi@x0 + (phi - np.identity(2*n))@np.linalg.solve(A, b))

    # Extract displacements
    (U, _) = np.split(np.array(X), 2, axis=1)
    
    return (t, U)

def simulate_odeint(t_end, n_out):
    # First order ODE with y = [u, v]
    M_inv = np.linalg.inv(M)
    def f(t, y):
        (u, v) = np.split(y, 2)
        a = M_inv @ (P(t) - D@v - Q(u))
        return np.concatenate((v, a))

    # Solve ODE with initial state zero
    y0 = np.concatenate((u0, v0))
    sol = solve_ivp(f, [0, t_end], y0)

    # Extract displacements
    (U, _) = np.split(sol.y, 2, axis=0)
    
    return (sol.t, U.T)

def simulate_central_difference(t_end, n_out):
    M_inv = np.linalg.inv(M)

    t = np.linspace(0, t_end, n_out)
    dt = t_end/n_out

    a0 = M_inv @ (P(0) - D@v0 - Q(u0))
    
    U = [u0 - dt*v0 + dt**2/2*a0, u0]    # Start with previous and current time step (previous is removed later)
    V = [v0 - dt*a0, v0]

    for ti in t[1:]:    # From 1 because state 0 is already known
        U.append(2*U[-1] - U[-2] + dt**2*M_inv @ (P(ti) -  D@V[-1] - Q(U[-1])))
        V.append((U[-1] - U[-2])/dt)

    return (t, np.array(U[1:]))    # Omit previous timestep

def simulate_newmark_linear(t_end, n_out):
    # Linear acceleration
    #beta = 1/6
    #gamma = 1/2

    # Average constant acceleration
    beta = 1/4
    gamma = 1/2

    dt = t_end/n_out

    M_eff = M + gamma*dt*D + beta*dt**2*K
    M_inv = np.linalg.inv(M_eff)

    t = [0.0]                                       # Initial time
    V = [v0]                                        # Initial velocity
    U = [u0]                                        # Initial displacements
    A = [np.linalg.solve(M, P(t[0]) - D@v0 - K@u0)]    # Initial acceleration

    while t[-1] < T:
        t_next = t[-1] + dt

        P_eff = P(t_next) - K@(U[-1] + dt*V[-1] + dt**2*(0.5 - beta)*A[-1]) - D@(V[-1] + (1 - gamma)*dt*A[-1])

        a_next = M_inv @ P_eff
        u_next = U[-1] + dt*V[-1] + dt**2*((0.5 - beta)*A[-1] + beta*a_next)
        v_next = V[-1] + dt*((1.0 - gamma)*A[-1] + gamma*a_next)

        t.append(t_next)
        U.append(u_next)
        V.append(v_next)
        A.append(a_next)

    return (t, np.array(U))

def simulate_newmark(t_end, n_out):
    # Linear acceleration
    #beta = 1/6
    #gamma = 1/2

    # Average constant acceleration
    beta = 1/4
    gamma = 1/2

    dt = t_end/n_out

    t = [0.0]                                               # Initial time
    U = [u0]                                                # Initial displacements
    V = [v0]                                                # Initial velocity
    A = [np.linalg.solve(M, P(t[0]) - D@V[0] - Q(U[0]))]    # Initial acceleration

    while t[-1] < T:
        t_next = t[-1] + dt

        def u_pred(a_next):
            return U[-1] + dt*V[-1] + dt**2*((0.5 - beta)*A[-1] + beta*a_next)
        
        def v_pred(a_next):
            return V[-1] + dt*((1 - gamma)*A[-1] + gamma*a_next)
        
        def f(a_next):
            return M@a_next + D@v_pred(a_next) + Q(u_pred(a_next)) - P(t_next)

        def Df(a_next):
            return M + gamma*dt*D + beta*dt**2*Kt(u_pred(a_next))

        # Solve for next accelerations
        a_next = fsolve(f, A[-1], fprime=Df)    # Use last acceleration as starting point
        u_next = u_pred(a_next)
        v_next = v_pred(a_next)

        t.append(t_next)
        U.append(u_next)
        V.append(v_next)
        A.append(a_next)

    return (t, np.array(U))

def simulate_generalized_alpha(t_end, n_out):
    # 0: Asymptotic annihilation case
    # 1: No numerical dissipation
    rho_inf = 1.0

    alpha_m = (2*rho_inf - 1)/(rho_inf + 1)
    alpha_f = rho_inf/(rho_inf + 1)
    beta = 1/4*(1 - alpha_m + alpha_f)**2
    gamma = 1/2 - alpha_m + alpha_f

    dt = t_end/n_out

    t = [0.0]                                               # Initial time
    U = [u0]                                                # Initial displacements
    V = [v0]                                                # Initial velocity
    A = [np.linalg.solve(M, P(t[0]) - D@V[0] - Q(U[0]))]    # Initial acceleration
    q = [Q(U[0])]                                           # Initial internal forces

    while t[-1] < T:
        # Next time point
        t_next = t[-1] + dt

        # Newmark approximations
        def u_pred(a_next):
            return U[-1] + dt*V[-1] + dt**2*((0.5 - beta)*A[-1] + beta*a_next)
        
        def v_pred(a_next):
            return V[-1] + dt*((1.0 - gamma)*A[-1] + gamma*a_next)
        
        # Alpha shift
        def u_alpha(a_next):
            return (1 - alpha_f)*u_pred(a_next) + alpha_f*U[-1]
        
        def v_alpha(a_next):
            return (1 - alpha_f)*v_pred(a_next) + alpha_f*V[-1]
        
        def a_alpha(a_next):
            return (1 - alpha_m)*a_next + alpha_m*A[-1]
        
        def q_alpha(a_next):
            return (1 - alpha_f)*Q(u_pred(a_next)) + alpha_f*q[-1]

        def p_alpha():
            return (1 - alpha_f)*P(t_next) + alpha_f*P(t[-1])

        def f(a_next):
            return M@a_alpha(a_next) + D@v_alpha(a_next) + q_alpha(a_next) - p_alpha()

        def Df(a_next):
            return (1 - alpha_m)*M + (1 - alpha_f)*(gamma*dt*D + beta*dt**2*Kt(u_pred(a_next)))

        # Solve for next accelerations
        a_next = fsolve(f, A[-1], fprime=Df)  # Use last acceleration as starting point
        u_next = u_pred(a_next)
        v_next = v_pred(a_next)

        t.append(t_next)
        U.append(u_next)
        V.append(v_next)
        A.append(a_next)
        q.append(Q(U[-1]))

    return (t, np.array(U))

def simulate_generalized_alpha_controlled(t_end, dt_start, n_period):
    # 0: Asymptotic annihilation case
    # 1: No numerical dissipation
    rho_inf = 1.0

    alpha_m = (2*rho_inf - 1)/(rho_inf + 1)
    alpha_f = rho_inf/(rho_inf + 1)
    beta = 1/4*(1 - alpha_m + alpha_f)**2
    gamma = 1/2 - alpha_m + alpha_f

    t = [0.0]                                               # Initial time
    U = [u0]                                                # Initial displacements
    V = [v0]                                                # Initial velocity
    A = [np.linalg.solve(M, P(t[0]) - D@V[0] - Q(U[0]))]    # Initial acceleration
    q = [Q(U[0])]                                           # Initial internal forces

    dt = dt_start
    while t[-1] < T:
        # Next time point
        t_next = t[-1] + dt

        # Newmark approximations
        def u_pred(a_next):
            return U[-1] + dt*V[-1] + dt**2*((0.5 - beta)*A[-1] + beta*a_next)
        
        def v_pred(a_next):
            return V[-1] + dt*((1.0 - gamma)*A[-1] + gamma*a_next)
        
        # Alpha shift
        def u_alpha(a_next):
            return (1 - alpha_f)*u_pred(a_next) + alpha_f*U[-1]
        
        def v_alpha(a_next):
            return (1 - alpha_f)*v_pred(a_next) + alpha_f*V[-1]
        
        def a_alpha(a_next):
            return (1 - alpha_m)*a_next + alpha_m*A[-1]
        
        def q_alpha(a_next):
            return (1 - alpha_f)*Q(u_pred(a_next)) + alpha_f*q[-1]

        def p_alpha():
            return (1 - alpha_f)*P(t_next) + alpha_f*P(t[-1])

        def f(a_next):
            return M@a_alpha(a_next) + D@v_alpha(a_next) + q_alpha(a_next) - p_alpha()

        def Df(a_next):
            return (1 - alpha_m)*M + (1 - alpha_f)*(gamma*dt*D + beta*dt**2*Kt(u_pred(a_next)))

        # Solve for next accelerations
        a_next = fsolve(f, A[-1], fprime=Df)  # Use last acceleration as starting point
        u_next = u_pred(a_next)
        v_next = v_pred(a_next)

        t.append(t_next)
        U.append(u_next)
        V.append(v_next)
        A.append(a_next)
        q.append(Q(U[-1]))

        # Method 1 (Rayleigh quotient)
        du = U[-1] - U[-2]
        w2 = abs(du.T @ Kt(U[-1]) @ du / du.T @ M @ du)
        dt_star = 2*pi/(sqrt(w2)*n_period)
        
        def fb(zeta):
            fp = 1.10
            zp = 1.25

            if zeta < 1/zp:
                return 1/fp
            if zeta > zp:
                return fp
            else:
                return 1

        def fc(zeta):
            zp = 1.4
            zm = 2.0

            if 1/zp < zeta < zp:
                return 1.0
            else:
                return min(zeta, zm)

        def fd(zeta):
            return min((1 + 0.1)*sqrt(zeta), 1.5)

        def fe(zeta):
            return 0.1*tanh(zeta - 1.0) + 1.0

        # Chrisfield - Limit increase by factor two, but no limit on decrease
        def ff(zeta):
            return min(zeta, 2)

        zeta = dt_star/dt
        dt = ff(zeta)*dt
        
        # Method 2 (Euler backwards)
        #e_target = 1e-4
        #e_actual = np.linalg.norm(U[-1] - U[-2] - dt*V[-1])
        #dt = dt*sqrt(e_target/e_actual)

    return (np.array(t), np.array(U))

# Simulate system
n_steps = 1000

(t_analytical, u_analytical) = simulate_analytical(T, n_steps)
(t_odeint, u_odeint) = simulate_odeint(T, n_steps)
(t_central, u_central) = simulate_central_difference(T, n_steps)
(t_linear, u_linear) = simulate_newmark_linear(T, n_steps)
(t_newmark, u_newmark) = simulate_newmark(T, n_steps)
(t_alpha, u_alpha) = simulate_generalized_alpha(T, n_steps)
(t_control, u_control) = simulate_generalized_alpha_controlled(T, 1e-6, 50)

print("Last displacements:")
for val in u_analytical[:,0]:
    print(val)

# Plot results

n_plots = min(n, 5)
fig, axs = plt.subplots(n_plots, 1, sharex=True, squeeze=False)

for i in range(0, n_plots):
    axs[i][0].plot(t_analytical, u_analytical[:,i], label="Reference")
    axs[i][0].plot(t_odeint, u_odeint[:,i], linestyle="--", label=f"Scipy odeint")
    axs[i][0].plot(t_central, u_central[:,i], linestyle="--", label=f"Central difference")
    axs[i][0].plot(t_linear, u_linear[:,i], linestyle="", marker="x", label=f"Newmark beta (linear)")
    axs[i][0].plot(t_newmark, u_newmark[:,i], linestyle="", marker="x", label=f"Newmark beta")
    axs[i][0].plot(t_alpha, u_alpha[:,i], linestyle="--", label=f"Generalized alpha")
    axs[i][0].plot(t_control, u_control[:,i], linestyle="--", label=f"Generalized alpha (controlled)")
    axs[i][0].set_ylabel(f"U_{i}")
    axs[i][0].grid()

axs[-1][0].set_xlabel("Time [s]")
plt.legend()

# Plot timestep

plt.figure()
plt.plot(t_odeint[1:], np.diff(t_odeint), marker="x", label=f"Scipy odeint ({len(t_odeint)} steps)")
plt.plot(t_control[1:], np.diff(t_control), marker="x", label=f"Generalized alpha controlled ({len(t_control)} steps)")
plt.grid()
plt.legend()

# Show plots
plt.show()
