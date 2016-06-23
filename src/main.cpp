#include "fem/System.hpp"
#include "fem/elements/BarElement.hpp"
#include "fem/elements/MassElement.hpp"

#include <iostream>

void test_harmonic_oscillator()
{
    // https://de.wikipedia.org/wiki/Schwingung#Linear_ged.C3.A4mpfte_Schwingung
    double l = 1.0;
    double k = 100.0;
    double d = 10.0;
    double m = 5.0;

    double s0 = 0.1;   // Initial displacement

    System system;
    Node node_a = system.create_node({{   0.0, 0.0, 0.0}}, {{Dof::Type::Fixed, Dof::Type::Fixed, Dof::Type::Fixed}});
    Node node_b = system.create_node({{l + s0, 0.0, 0.0}}, {{Dof::Type::Active, Dof::Type::Fixed, Dof::Type::Fixed}});


    BarElement element1(node_a, node_b, l, l*k, l*d, 0.0);
    MassElement element2(node_b, m, 0.0);
    system.add_element(element1);
    system.add_element(element2);

    // Constants for the analytical solution
    double delta = d/(2.0*m);                                // Decay constant
    double omega0 = std::sqrt(k/m);                          // Natural frequency of the undamped system
    double omega = std::sqrt(omega0*omega0 - delta*delta);   // Natural frequency
    double T = 2.0*M_PI/omega;                                 // Period length

    //assert!(delta < omega0);                      // Make sure the system is underdamped  // Todo: Reintroduce this check

    system.solve_dynamics(1e-5, [&]()
    {
        // Time
        double t = system.get_time();

        // Numerical solution
        double s_num = system.get_u()(node_b.x) - l;
        double v_num = system.get_v()(node_b.x);

        // Analytical solution
        double A = delta/omega*s0;
        double B = s0;
        double s_ref = std::exp(-delta*t)*(A*std::sin(omega*t) + B*std::cos(omega*t));
        double v_ref = -std::exp(-delta*t)*((omega*B + delta*A)*std::sin(omega*t) + (delta*B - omega*A)*std::cos(omega*t));

        // Error
        double error_s = std::abs(s_num - s_ref);
        double error_v = std::abs(v_num - v_ref);

        //std::cout << error_s << "\n";

        return t < T;
    });
}

int main()
{
    test_harmonic_oscillator();


    return 0;
}

/*
#include "gui/MainWindow.hpp"

#include <QtWidgets>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
*/

/*
// Todo: Turn into test cases
#include "fem/system.hpp"
#include <iostream>

int main()
{
    std::array<Dof, 3> dofs{{{Dof::Active, 0}, {Dof::Active, 2}, {Dof::Active, 4}}};

    // Test VectorView
    {
        VectorXd vec = VectorXd::Zero(10);
        VectorAccess<VectorXd> acc(&vec, nullptr);

        // Write
        {
            VectorView<VectorXd> view(acc);

            // Scalar
            view(dofs)(0) += 3.6;
            view(dofs)(1) += 4.6;
            view(dofs[2]) += 5.6;

            // Vector
            view(dofs) += Vector<3>{1.0, 2.0, 3.0};
        }

        // Read
        {
            VectorView<VectorXd> view(acc);

            // Scalar

            // Vector
            Vector<3> u = view(dofs);
            std::cout << u << "\n\n";
        }

    }

    // Test MatrixView
    {
        MatrixXd mat = MatrixXd::Zero(10, 10);
        MatrixAccess<MatrixXd> acc(mat);

        // Write
        {
            MatrixView<MatrixXd> view(acc);

            // Scalar
            view(dofs)(0, 0) += 3.6;
            view(dofs)(1, 1) += 4.6;
            view(dofs[2], dofs[2]) += 5.6;

            // Matrix
            Matrix<3> m;
            m << 1.0, 2.0, 3.0,
                 4.0, 5.0, 6.0,
                 7.0, 8.0, 9.0;

            view(dofs) += m;
        }

        // Read
        {
            MatrixView<MatrixXd> view(acc);

            // Scalar

            // Matrix
            Matrix<3> u = view(dofs);
            std::cout << u << "\n\n";
        }
    }

    return 0;
}
*/
