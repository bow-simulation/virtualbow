#include "BarElement.hpp"

BarElement::BarElement(System& system, Node node0, Node node1, double L, double EA, double etaA, double rhoA)
    : ElementBase(system, {node0.x, node0.y, node1.x, node1.y}),
      L(L),
      EA(EA),
      etaA(etaA),
      rhoA(rhoA)
{

}

double BarElement::get_length() const
{
    return L;
}

void BarElement::set_length(double L)
{
    this->L = L;
}

void BarElement::set_stiffness(double EA)
{
    this->EA = EA;
}

void BarElement::set_damping(double etaA)
{
    this->etaA = etaA;
}

double BarElement::get_normal_force() const
{
    BarElementState state = get_state();
    return EA/L*(state.L_new - L) + etaA/L*state.L_dot;
}

BarElementState BarElement::compute_state(const Vector<4>& u, const Vector<4>& v) const
{
    double dx = u(2) - u(0);
    double dy = u(3) - u(1);
    double L_new = std::hypot(dx, dy);

    double dvx = v(2) - v(0);
    double dvy = v(3) - v(1);
    double L_dot = (dx*dvx + dy*dvy)/L_new;

    return {dx, dy, L_new, dvx, dvy, L_dot};
}

Vector<4> BarElement::get_mass_matrix() const
{
    double m = 0.5*rhoA*L;
    return {m, m, m, m};
}

Matrix<4> BarElement::get_tangent_stiffness_matrix() const
{
    BarElementState state = get_state();

    double dx = state.dx;
    double dy = state.dy;
    double L_new = state.L_new;

    double c0 = EA*(L_new - L)/(L_new*L);
    double c1 = EA/(L_new*L_new*L_new);

    Matrix<4, 4> K;
    K << c1*dx*dx + c0,       c1*dx*dy, -c1*dx*dx - c0,      -c1*dx*dy,
              c1*dx*dy,  c1*dy*dy + c0,      -c1*dx*dy, -c1*dy*dy - c0,
        -c1*dx*dx - c0,      -c1*dx*dy,  c1*dx*dx + c0,       c1*dx*dy,
             -c1*dx*dy, -c1*dy*dy - c0,       c1*dx*dy,  c1*dy*dy + c0;

    return K;
}

Matrix<4> BarElement::get_tangent_damping_matrix() const
{
    Matrix<4, 4> D;
    D << etaA/L,     0.0, -etaA/L,    0.0,
            0.0,  etaA/L,     0.0,-etaA/L,
        -etaA/L,     0.0,  etaA/L,    0.0,
            0.0, -etaA/L,     0.0, etaA/L;

    return D;
}

Vector<4> BarElement::get_internal_forces() const
{
    BarElementState state = get_state();
    double N = get_normal_force();

    return N/state.L_new*Vector<4>{-state.dx, -state.dy, state.dx, state.dy};
}

double BarElement::get_potential_energy() const
{
    BarElementState state = get_state();
    return 0.5*EA/L*(state.L_new - L)*(state.L_new - L);
}
