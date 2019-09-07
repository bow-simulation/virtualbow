#pragma once
#include "numerics/Eigen.hpp"
#include "numerics/Integration.hpp"

class BeamUtils
{
public:
    // Calculates the analytical stiffness matrix of a curved beam with varying cross section properties
    // by using Castigliano's theorem and numerical integration. Method inspired by [1]. The stiffness matrix
    // corresponds to the displacements {x0, y0, phi0, x1, y1, phi1} of the start and endpoints of the beam,
    // respecitvely (same coordinate system as r).
    //
    // [1] E. Marotta, P.Salvini: Analytical Stiffness Matrix for Curved Metal Wires,
    // Procedia Structural Integrity, Volume 8, 2018, Pages 43-55
    //
    // r: s -> [x, y, phi], Function describing the shape of the beam segment with arc length s in [l0, l1]
    // C: s -> [[Cee, Cek], [Cek, Ckk]], Function describing the material properties along the beam
    template<class F1, class F2>
    static Matrix<6, 6> stiffness_matrix(const F1& r, const F2& C, double s0, double s1)
    {
        Vector<3> r0 = r(s0);
        Vector<3> r1 = r(s1);

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

        Matrix<3, 3> Kbb_inv = AdaptiveSimpson::integrate<Matrix<3, 3>>(f, s0, s1, 1e-9);    // Todo: Magic number
        Matrix<3, 3> Kaa_inv = Sba.transpose()*Kbb_inv*Sba;

        Matrix<3, 3> Kaa = Kaa_inv.inverse();
        Matrix<3, 3> Kbb = Kbb_inv.inverse();
        Matrix<3, 3> Kba = Sba*Kaa;

        return (Matrix<6, 6>() << Kaa, Kba.transpose(),
                                  Kba, Kbb).finished();
    }

    struct MassProperties {
        double m;
        double I;
        double x;
        double y;
    };

    // Calculates the mass properties of a curved beam with varying cross section properties.
    //
    // m    = integrate rhoA(t) dt from s0 to s1
    // x_cg = 1/m * integrate x(t)*rhoA(t) dt from s0 to s1
    // y_cg = 1/m * integrate y(t)*rhoA(t) dt from s0 to s1
    // I_cg = integrate ((x(t) - x_cg)^2 + (y(t) - y_cg)^2)*rhoA(t) dt from s0 to s1
    //
    //    r: s -> [x, y, phi], Function describing the shape of the beam segment with arc length s in [l0, l1]
    // rhoA: s -> double, Function describing the linear density along the beam
    template<class F1, class F2>
    static MassProperties mass_properties(const F1& r, const F2& rhoA, double s0, double s1)
    {
        const double EPSILON = 1e-9;    // Magic number

        double m_cg = AdaptiveSimpson::integrate<double>(rhoA, s0, s1, EPSILON);

        double x_cg = AdaptiveSimpson::integrate<double>([&](double s){
            return rhoA(s)*r(s)[0];
        }, s0, s1, EPSILON)/m_cg;

        double y_cg = AdaptiveSimpson::integrate<double>([&](double s){
            return rhoA(s)*r(s)[1];
        }, s0, s1, EPSILON)/m_cg;

        double I_cg = AdaptiveSimpson::integrate<double>([&](double s){
            Vector<3> rs = r(s);
            return rhoA(s)*(pow(rs[0] - x_cg, 2) + pow(rs[1] - y_cg, 2));
        }, s0, s1, EPSILON);

        return {
            .m = m_cg,
            .I = I_cg,
            .x = x_cg,
            .y = y_cg
        };
    }
};
