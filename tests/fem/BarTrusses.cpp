#include "fem/System.hpp"
#include "fem/StaticSolver.hpp"
#include "fem/elements/BarElement.hpp"
#include "numerics/Linspace.hpp"

#include <catch.hpp>
#include <iostream>

TEST_CASE("small-deformation-bar-truss")
{
    double L = 0.5;
    double EA = 21000.0;    // Steel rod, 1cm x 1cm
    double F = 10.0;

    System system;
    Node node_01 = system.create_node({false, false, false}, {  0.0, 0.0, 0.0});
    Node node_02 = system.create_node({ true,  true, false}, {    L, 0.0, 0.0});
    Node node_03 = system.create_node({ true,  true, false}, {2.0*L, 0.0, 0.0});
    Node node_04 = system.create_node({ true,  true, false}, {3.0*L, 0.0, 0.0});
    Node node_05 = system.create_node({ true, false, false}, {4.0*L, 0.0, 0.0});
    Node node_06 = system.create_node({ true,  true, false}, {  0.0,   L, 0.0});
    Node node_07 = system.create_node({ true,  true, false}, {    L,   L, 0.0});
    Node node_08 = system.create_node({ true,  true, false}, {2.0*L,   L, 0.0});
    Node node_09 = system.create_node({ true,  true, false}, {3.0*L,   L, 0.0});
    Node node_10 = system.create_node({ true,  true, false}, {4.0*L,   L, 0.0});

    system.mut_elements().add(BarElement(system, node_01, node_02, L, EA, 0.0));
    system.mut_elements().add(BarElement(system, node_02, node_03, L, EA, 0.0));
    system.mut_elements().add(BarElement(system, node_03, node_04, L, EA, 0.0));
    system.mut_elements().add(BarElement(system, node_04, node_05, L, EA, 0.0));

    system.mut_elements().add(BarElement(system, node_06, node_07, L, EA, 0.0));
    system.mut_elements().add(BarElement(system, node_07, node_08, L, EA, 0.0));
    system.mut_elements().add(BarElement(system, node_08, node_09, L, EA, 0.0));
    system.mut_elements().add(BarElement(system, node_09, node_10, L, EA, 0.0));

    system.mut_elements().add(BarElement(system, node_01, node_06, L, EA, 0.0));
    system.mut_elements().add(BarElement(system, node_02, node_07, L, EA, 0.0));
    system.mut_elements().add(BarElement(system, node_03, node_08, L, EA, 0.0));
    system.mut_elements().add(BarElement(system, node_04, node_09, L, EA, 0.0));
    system.mut_elements().add(BarElement(system, node_05, node_10, L, EA, 0.0));

    system.mut_elements().add(BarElement(system, node_01, node_07, std::sqrt(2)*L, EA, 0.0));
    system.mut_elements().add(BarElement(system, node_07, node_03, std::sqrt(2)*L, EA, 0.0));
    system.mut_elements().add(BarElement(system, node_03, node_09, std::sqrt(2)*L, EA, 0.0));
    system.mut_elements().add(BarElement(system, node_09, node_05, std::sqrt(2)*L, EA, 0.0));

    system.set_p(node_02.y, -F);
    system.set_p(node_04.y, -F);

    StaticSolverLC solver(system);
    solver.solve();

    double s_num = -system.get_u(node_03.y);
    double s_ref = (4.0 + 2.0*std::sqrt(2))*F*L/EA;

    REQUIRE(std::abs(s_num - s_ref) < 1e-6);
}

class StaticSolverBase
{
public:

protected:
    virtual void eval_constraint(const VectorXd& u, double lambda, double& c, VectorXd& dcdu, double& dcdl) const = 0;
    virtual void eval_load_vector(double lambda, VectorXd& p, VectorXd& dpdl) const = 0;

private:
    mutable VectorXd returnc;
};

struct SolverSettings
{
    unsigned iter_max = 150;
    unsigned iter_ref = 5;
    double epsilon_q = 1e-6;
    double epsilon_c = 1e-6;
    bool full_update = true;
    bool line_search = false;
};

void solve_statics_dc(System& system, Dof dof, double u_target, SolverSettings settings = SolverSettings())
{
    auto eval_constraint = [&](const VectorXd& u, double lambda, double& c, VectorXd& dcdu, double& dcdl) {
        c = u(dof.index) - u_target;
        dcdu = unit_vector(system.dofs(), dof.index);
        dcdl = 0.0;
    };

    VectorXd p_initial = system.get_p();
    auto eval_load_vector = [&](double lambda, VectorXd& p, VectorXd& dpdl) {
        p = p_initial + lambda*unit_vector(system.dofs(), dof.index);
        dpdl = unit_vector(system.dofs(), dof.index);
    };

    Eigen::LDLT<MatrixXd> decomp;
    VectorXd delta_q(system.dofs());
    VectorXd delta_u(system.dofs());
    VectorXd alpha(system.dofs());
    VectorXd beta(system.dofs());

    double lambda = 0.0;

    double c;
    double dcdl;
    VectorXd dcdu(system.dofs());

    VectorXd p(system.dofs());
    VectorXd dpdl(system.dofs());
    eval_load_vector(lambda, p, dpdl);

    double energy_q_0;
    double energy_c_0;
    bool convergence = false;

    for(unsigned i = 0; (i < settings.iter_max) && !convergence; ++i)
    {
        // Calculate factorization of the system's tangent stiffness matrix
        // Always in the first iteration, in subsequent ones only for the full Newton-Raphson method
        if(i == 0 || settings.full_update) {
            decomp.compute(system.get_K());
            if(decomp.info() != Eigen::Success)
                throw std::runtime_error("Decomposition failed!");
        }

        // Calculate out of balance loads and auxiliary vectors alpha and beta
        delta_q = system.get_q() - p;
        alpha = -decomp.solve(delta_q);
        beta = decomp.solve(dpdl);

        // Evaluate constraint and calculate change in lambda
        eval_constraint(system.get_u(), lambda, c, dcdu, dcdl);
        double delta_l = -(c + dcdu.transpose()*alpha)/(dcdl + dcdu.transpose()*beta);
        delta_u = alpha + delta_l*beta;

        // Check for convergence
        double energy_q_i = delta_u.transpose()*delta_q;
        double energy_c_i = delta_l*c;

        if(i == 0) {
            energy_q_0 = energy_q_i;
            energy_c_0 = energy_c_i;
        }

        if((energy_q_0 == 0.0 || energy_q_i/energy_q_0 < settings.epsilon_q)
            && (energy_c_0 == 0.0 || energy_c_i/energy_c_0 < settings.epsilon_c))
        {
            convergence = true;
        }

        // Apply changes
        lambda = lambda + delta_l;
        system.set_u(system.get_u() + delta_u);
        eval_load_vector(lambda, p, dpdl);
        system.set_p(p);
    }

    if(!convergence) {
        throw std::runtime_error("Iterations exceeded!");
    }
}

#include <chrono>

// Todo: Why does the displacement control not allow passing the point 0.5*H?
// Read section on displacement control in 'Nonlinear Finite Element Analysis of Solids and Structures (René De Borst,Mike A. Crisfield,Joris J. C.)
TEST_CASE("large-deformation-bar-truss")
{
    double H = 1.0;
    double EA = 10000.0;

    System system;

    Node node01 = system.create_node({false, false, false}, {  0.0, 0.0, 0.0});
    Node node02 = system.create_node({false,  true, false}, {    H,   H, 0.0});
    Node node03 = system.create_node({false, false, false}, {2.0*H, 0.0, 0.0});

    system.mut_elements().add(BarElement(system, node01, node02, M_SQRT2*H, EA, 0.0));
    system.mut_elements().add(BarElement(system, node02, node03, M_SQRT2*H, EA, 0.0));

    auto t1 = std::chrono::high_resolution_clock::now();

    StaticSolverDC solver(system, node02.y);
    for(double d: Linspace<double>(H, -H, 10000))
    {
        // Numerical solution
        solve_statics_dc(system, node02.y, d);

        double s = system.get_u(node02.y);
        double F_num = system.get_p(node02.y);

        // Analytical solution
        double L0 = M_SQRT2*H;
        double L = hypot(H, s);
        double F_ref = 2.0*EA*s*(L - L0)/(L*L0);

        std::cout << F_ref << ", " << F_num << "\n";

        // Error
        REQUIRE(std::abs(F_num - F_ref) < 1e-9);
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    std::cout << "Duration: " << duration << "\n";
}
