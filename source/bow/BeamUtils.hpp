#pragma once
#include "numerics/Eigen.hpp"
#include "numerics/Integration.hpp"

// Calculates the analytical stiffness matrix of a curved beam with varying cross section properties
// by using Castigliano's theorem and numerical integration. Method inspired by [1].
//
// [1] E. Marotta, P.Salvini: Analytical Stiffness Matrix for Curved Metal Wires,
// Procedia Structural Integrity, Volume 8, 2018, Pages 43-55
//
// r: s -> [x, y, phi], Function describing the shape of the beam segment with arc length s in [l0, l1]
// C: s -> [[Cee, Cek], [Cek, Ckk]], Function describing the material properties along the beam

template<class F1, class F2>
Matrix<6, 6> beam_stiffness_matrix(const F1& r, const F2& C, double l0, double l1)
{
    Vector<3> r0 = r(l0);
    Vector<3> r1 = r(l1);

    Matrix<3, 3> Sba;
    Sba << -1.0,  0.0, 0.0,
            0.0, -1.0, 0.0,
            r0[1]-r1[1], r1[0]-r0[0], -1.0;

    auto Hb = [&](double s) -> Matrix<2, 3> {
        auto rs = r(s);
        return (Matrix<2, 3>() << cos(rs[2]), sin(rs[2]), 0.0,
                                  rs[1]-r1[1], r1[0]-rs[0], 1.0).finished();
    };

    auto f = [&](double s) -> Matrix<3, 3> {
        auto H = Hb(s);
        return H.transpose()*C(s).inverse()*H;
    };

    Matrix<3, 3> Kbb_inv = AdaptiveSimpson::integrate<Matrix<3, 3>>(f, l0, l1, 1e-9);    // Todo: Magic number
    Matrix<3, 3> Kaa_inv = Sba.transpose()*Kbb_inv*Sba;

    Matrix<3, 3> Kaa = Kaa_inv.inverse();
    Matrix<3, 3> Kbb = Kbb_inv.inverse();
    Matrix<3, 3> Kba = Sba*Kaa;

    return (Matrix<6, 6>() << Kaa, Kba.transpose(),
                              Kba, Kbb).finished();
}
