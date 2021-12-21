#pragma once
#include "Node.hpp"
#include "solver/numerics/Optimization.hpp"
#include "solver/numerics/EigenTypes.hpp"

class System;

// Todo: Make constraint function a member with templated type instead of using inheritance.
class StaticSolver {
public:
    // Todo: State for failed line search
    struct Info {
        enum { Success, DecompFailed, NoConvergence } outcome;
        unsigned iterations;
    };

    StaticSolver(System& system);

protected:
    Info solve();
    virtual void constraint(const VectorXd& u, double lambda, double& c, double& dcdl, VectorXd& dcdu) const = 0;

private:
    System& system;

    const unsigned max_iter = 150;    // Todo: Magic number
    const double epsilon = 1e-5;      // Todo: Magic number

    Eigen::LDLT<MatrixXd> decomp;
    VectorXd delta_q;
    VectorXd delta_u;
    VectorXd alpha;
    VectorXd beta;

    double c;
    double dcdl;
    VectorXd dcdu;
};

class StaticSolverLC: public StaticSolver
{
public:
    StaticSolverLC(System& system);
    Info solve();

protected:
    void constraint(const VectorXd& u, double lambda, double& c, double& dcdl, VectorXd& dcdu) const override;
};

class StaticSolverDC: public StaticSolver
{
public:
    StaticSolverDC(System& system, Dof dof);
    Info solve(double displacement);

protected:
    virtual void constraint(const VectorXd& u, double lambda, double& c, double& dcdl, VectorXd& dcdu) const override;

private:
    Dof dof;
    double target;
    VectorXd e_dof;

    static VectorXd unit_vector(size_t n, size_t i);
};
