#pragma once
#include "solver/fem/ElementBase.hpp"
#include "solver/fem/Node.hpp"
#include "solver/numerics/Eigen.hpp"
#include <array>

struct ContactForce
{
public:
    ContactForce(double k, double epsilon);
    double force(double e) const;
    double stiffness(double e) const;
    double energy(double e) const;

private:
    double k;
    double epsilon;
};

struct ContactElementState
{
    Vector<8> u = Vector<8>::Zero();
    double e = 0.0;
    double a1 = 0.0;
    double a2 = 0.0;
    double a3 = 0.0;
    double a4 = 0.0;
    double b1 = 0.0;
    double b2 = 0.0;
    Vector<8> v1 = Vector<8>::Zero();
    Vector<8> v2 = Vector<8>::Zero();
    Vector<8> Da1 = Vector<8>::Zero();
    Vector<8> Da2 = Vector<8>::Zero();
    Vector<8> Da3 = Vector<8>::Zero();
    Vector<8> Da4 = Vector<8>::Zero();
    Vector<8> De = Vector<8>::Zero();
};

class ContactElement: public ElementBase<8, ContactElementState>
{
public:
    ContactElement(System& system, Node node0, Node node1, Node node2, double h0, double h1, ContactForce f);

    virtual ContactElementState compute_state(const Vector<8>& u, const Vector<8>& v) const override;
    virtual Vector<8> get_mass_matrix() const override;
    virtual Matrix<8> get_tangent_stiffness_matrix() const override;
    virtual Matrix<8> get_tangent_damping_matrix() const override;
    virtual Vector<8> get_internal_forces() const override;
    virtual double get_potential_energy() const override;

private:
    double h0;
    double h1;
    ContactForce f;
};
