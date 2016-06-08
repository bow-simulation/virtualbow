#include "fem/view.hpp"

#include <Eigen/Dense>

#include <exception>
#include <iostream>

using Eigen::VectorXd;
using Eigen::MatrixXd;

struct Dof
{
    enum{Active, Fixed} type;
    size_t index;
};

class VectorAccess
{
private:
    VectorXd& vec;

public:
    VectorAccess(VectorXd& vec)
        : vec(vec)
    {

    }

    double get(Dof dof) const
    {
        switch(dof.type)
        {
        case Dof::Active:
            return vec(dof.index);
        case Dof::Fixed:
            return 0.0;
        }
    }

    void add(Dof dof, double val)
    {
        switch(dof.type)
        {
        case Dof::Active:
            vec(dof.index) += val;
            return;
        case Dof::Fixed:
            return;
        }
    }
};

class MatrixAccess
{
private:
    MatrixXd& mat;

public:
    MatrixAccess(MatrixXd& mat)
        : mat(mat)
    {

    }

    double get(Dof dof_row, Dof dof_col) const
    {
        if(dof_row.type == Dof::Active && dof_col.type == Dof::Active)
        {
            return mat(dof_row.index, dof_col.index);
        }
    }

    void add(Dof dof_row, Dof dof_col, double val)
    {
        if(dof_row.type == Dof::Active && dof_col.type == Dof::Active)
        {
            mat(dof_row.index, dof_col.index) += val;
        }
    }
};

int main()
{
    std::array<Dof, 3> dofs{{{Dof::Active, 0}, {Dof::Active, 2}, {Dof::Active, 4}}};

    // Test VectorView
    {
        VectorXd vec = VectorXd::Zero(10);
        VectorAccess acc(vec);

        // Write
        {
            VectorView<VectorAccess, Dof> view(acc);

            // Scalar
            view(dofs)(0) += 3.6;
            view(dofs)(1) += 4.6;
            view(dofs[2]) += 5.6;

            // Vector
            view(dofs) += Vector<3>{1.0, 2.0, 3.0};
        }

        // Read
        {
            const VectorView<VectorAccess, Dof> view(acc);

            // Scalar

            // Vector
            Vector<3> u = view(dofs);
            std::cout << u << "\n\n";
        }
    }

    // Test MatrixView
    {
        MatrixXd mat = MatrixXd::Zero(10, 10);
        MatrixAccess acc(mat);

        // Write
        {
            MatrixView<MatrixAccess, Dof> view(acc);

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
            const MatrixView<MatrixAccess, Dof> view(acc);

            // Scalar

            // Matrix
            Matrix<3> u = view(dofs);
            std::cout << u << "\n\n";
        }
    }


    return 0;
}
