#include "bow/BeamUtils.hpp"
#include "numerics/Linspace.hpp"
#include <catch.hpp>

// Returns the stiffness matrix of a straight Euler-Bernoulli beam with orientation angle alpha
Matrix<6, 6> element_stiffness_matrix(double EA, double EI, double L, double alpha)
{
    Matrix<6, 6> K, T;
    K << EA/L,              0,           0, -EA/L,              0,           0,
            0,  12*EI/(L*L*L),  6*EI/(L*L),     0, -12*EI/(L*L*L),  6*EI/(L*L),
            0,     6*EI/(L*L),      4*EI/L,     0,    -6*EI/(L*L),      2*EI/L,
        -EA/L,              0,           0,  EA/L,              0,           0,
            0, -12*EI/(L*L*L), -6*EI/(L*L),     0,  12*EI/(L*L*L), -6*EI/(L*L),
            0,     6*EI/(L*L),      2*EI/L,     0,    -6*EI/(L*L),      4*EI/L;

    T << cos(alpha), sin(alpha), 0,           0,          0, 0,
        -sin(alpha), cos(alpha), 0,           0,          0, 0,
                  0,          0, 1,           0,          0, 0,
                  0,          0, 0,  cos(alpha), sin(alpha), 0,
                  0,          0, 0, -sin(alpha), cos(alpha), 0,
                  0,          0, 0,           0,          0, 1;

    return T.transpose()*K*T;
}

// Approximates the stiffness matrix of a curved, non-uniform beam segment by using a number of straight elements
// and reducing the resulting stiffness matrix to the relevant displacements (first and last node)
template<class F1, class F2, class F3>
Matrix<6, 6> beam_stiffness_matrix_fem(const F1& r, const F2& EA, const F3& EI, double l0, double l1, unsigned n_elements)
{
    assert(n_elements >= 2);

    unsigned n_nodes = n_elements + 1;
    unsigned n_dofs = 3*n_nodes;

    // Create total stiffness matrix for all nodes
    MatrixXd K_total = MatrixXd::Zero(n_dofs, n_dofs);

    std::vector<double> s = Linspace<double>(l0, l1, n_nodes).collect();
    for(unsigned i = 0; i < n_elements; ++i)
    {
        Vector<3> r_prev = r(s[i]);
        Vector<3> r_next = r(s[i+1]);

        double dx = r_next[0] - r_prev[0];
        double dy = r_next[1] - r_prev[1];

        K_total.block(3*i, 3*i, 6, 6) += element_stiffness_matrix(
            0.5*(EA(s[i]) + EA(s[i+1])),    // Element longitudinal stiffness
            0.5*(EI(s[i]) + EI(s[i+1])),    // Element bending stiffness
            hypot(dx, dy),                  // Element legth
            atan2(dy, dx)                   // Element orientation angle
        );
    }

    /*
    Reducing the total stiffness matrix to forces and displacements of the first and last node.
    K_total is partitioned like this:

    | F1 |   | K11    K12    K13 |   | U1 |
    | 0  | = | K12^T  K22    K23 | * | U2 |
    | F3 |   | K13^T  K23^T  K33 |   | U3 |

    F1, U1: Forces and displacements of the first node
    F3, U3: Forces and displacements of the last node
    U2: Equilibrium displacements of all nodes inbetween.

    Second block equation:

    0 = K12^T*U1 + K22*U2 + K23*U3
      => U2 = -K22^(-1)*(K12^T*U1 + K23*U3)

    First and third equations:

    F1 = K11*U1 + K12*U2 + K13*U3
       = K11*U1 - K12*K22^(-1)*(K12^T*U1 + K23*U3) + K13*U3
       = (K11 - K12*K22^(-1)*K12^T)*U1 + (K13 - K12*K22^(-1)*K23)*U3

    F3 = K13^T*U1 + K23^T*U2 + K33*U3
       = K13^T*U1 - K23^T*K22^(-1)*(K12^T*U1 + K23*U3) + K33*U3
       = (K13^T - K23^T*K22^(-1)*K12^T)*U1 + (K33 - K23^T*K22^(-1)*K23)*U3

    Giving the reduced stiffness matrix:

    | F1 |   | K11 - K12*K22^(-1)*K12^T      K13 - K12*K22^(-1)*K23   |   | U1 |
    |    | = |                                                        | * |    |
    | F3 |   | K13^T - K23^T*K22^(-1)*K12^T  K33 - K23^T*K22^(-1)*K23 |   | U3 |
    */

    auto K11 = K_total.block(0, 0, 3, 3);
    auto K22 = K_total.block(3, 3, n_dofs-6, n_dofs-6);
    auto K33 = K_total.block(n_dofs-3, n_dofs-3, 3, 3);
    auto K12 = K_total.block(0, 3, 3, n_dofs-6);
    auto K13 = K_total.block(0, n_dofs-3, 3, 3);
    auto K23 = K_total.block(3, n_dofs-3, n_dofs-6, 3);

    auto K22_inv = K22.inverse();
    Matrix<6, 6> K_reduced;
    K_reduced << K11-K12*K22_inv*K12.transpose(), K13-K12*K22_inv*K23,
                 K13.transpose()-K23.transpose()*K22_inv*K12.transpose(), K33-K23.transpose()*K22_inv*K23;

    return K_reduced;
}

// Compare stiffness matrix of a straight beam with non-zero orientation to the analytical solution
TEST_CASE("stiffness-matrix-straight")
{
    double EA = 1e6;
    double EI = 1e3;
    double L = 1.5;
    double alpha = M_PI/4.0;

    auto r = [&](double s) {
        return (Vector<3>() << s*cos(alpha), s*sin(alpha), alpha).finished();
    };

    auto C = [&](double s) {
        return (Matrix<2, 2>() << EA, 0.0, 0.0, EI).finished();
    };

    Matrix<6, 6> K_num = BeamUtils::stiffness_matrix(r, C, 0.0, L);
    Matrix<6, 6> K_ref = element_stiffness_matrix(EA, EI, L, alpha);
    REQUIRE(K_num.isApprox(K_ref, 1e-8));
}

// Compare stiffness matrix of a quarter-circle with uniform section properties to the fem approximation
TEST_CASE("stiffness-matrix-curved")
{
    double EA = 1e6;
    double EI = 1e3;
    double R = 0.1;
    double L = M_PI_2*R;

    auto r = [&](double s) {
        return (Vector<3>() << R*sin(s/R), R*(1.0 - cos(s/R)), s/R).finished();
    };

    auto EA_fn = [&](double s) {
        return EA;
    };

    auto EI_fn = [&](double s) {
        return EI;
    };

    auto C = [&](double s) {
        return (Matrix<2, 2>() << EA_fn(s), 0.0, 0.0, EI_fn(s)).finished();
    };

    Matrix<6, 6> K_num = BeamUtils::stiffness_matrix(r, C, 0.0, L);
    Matrix<6, 6> K_ref = beam_stiffness_matrix_fem(r, EA_fn, EI_fn, 0.0, L, 500);
    REQUIRE(K_num.isApprox(K_ref, 1e-4));
}

// Compare stiffness matrix of a half-circle with varying section properties to the fem approximation
TEST_CASE("stiffness-matrix-curved-non-uniform")
{
    double EA_0 = 1e6;
    double EI_0 = 1e3;

    double EA_1 = 0.5e6;
    double EI_1 = 0.25e3;

    double R = 0.1;
    double L = M_PI*R;

    auto r = [&](double s) {
        return (Vector<3>() << R*sin(s/R), R*(1.0 - cos(s/R)), s/R).finished();
    };

    auto EA_fn = [&](double s) {
        return EA_0*(1.0 - s/L) + EA_1*s/L;
    };

    auto EI_fn = [&](double s) {
        return EI_0*(1.0 - s/L) + EI_1*s/L;
    };

    auto C = [&](double s) {
        return (Matrix<2, 2>() << EA_fn(s), 0.0, 0.0, EI_fn(s)).finished();
    };

    Matrix<6, 6> K_num = BeamUtils::stiffness_matrix(r, C, 0.0, L);
    Matrix<6, 6> K_ref = beam_stiffness_matrix_fem(r, EA_fn, EI_fn, 0.0, L, 500);
    REQUIRE(K_num.isApprox(K_ref, 1e-4));
}

TEST_CASE("mass-properties-straight")
{
    double rhoA = 1e3;
    double L = 1.5;
    double alpha = M_PI/4.0;

    auto r = [&](double s) {
        return (Vector<3>() << s*cos(alpha), s*sin(alpha), alpha).finished();
    };

    auto rhoA_fn = [&](double s) {
        return rhoA;
    };

    BeamUtils::MassProperties properties = BeamUtils::mass_properties(r, rhoA_fn, 0.0, L);

    double m_ref = rhoA*L;
    double I_ref = rhoA*L*L*L/12.0;
    double x_ref = r(0.5*L)[0];
    double y_ref = r(0.5*L)[1];

    REQUIRE(std::abs(properties.m - m_ref) < 1e-9);
    REQUIRE(std::abs(properties.I - I_ref) < 1e-9);
    REQUIRE(std::abs(properties.x - x_ref) < 1e-9);
    REQUIRE(std::abs(properties.y - y_ref) < 1e-9);
}

#include <iostream>
TEST_CASE("mass-properties-circle")
{
    double rhoA = 1e3;
    double R = 1.5;
    double L = 2.0*M_PI*R;

    auto r = [&](double s) {
        return (Vector<3>() << R*cos(s/R), R*sin(s/R), s/R).finished();
    };

    auto rhoA_fn = [&](double s) {
        return rhoA;
    };

    BeamUtils::MassProperties properties = BeamUtils::mass_properties(r, rhoA_fn, 0.0, L);

    double m_ref = rhoA*L;
    double I_ref = m_ref*R*R;
    double x_ref = 0.0;
    double y_ref = 0.0;

    REQUIRE(std::abs(properties.m - m_ref) < 1e-9);
    REQUIRE(std::abs(properties.I - I_ref) < 1e-9);
    REQUIRE(std::abs(properties.x - x_ref) < 1e-9);
    REQUIRE(std::abs(properties.y - y_ref) < 1e-9);
}
