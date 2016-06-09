#include "fem/system.hpp"

#include <iostream>


int main()
{
    /*
    System sys;

    std::cout << sys.dofs();

    auto u = sys.get_u();
    */

    VectorXd vec = VectorXd::Zero(5);

    std::cout << vec << "\n\n";

    VectorXd unew;
    unew = (VectorXd(6) << vec, 1.0).finished();

    std::cout << unew << "\n\n";
}

/*
// Todo: Turn into test cases
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
