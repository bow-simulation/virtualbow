#pragma once
#include "InputData.hpp"
#include "../numerics/ArcCurve.hpp"
#include "../numerics/CubicSpline.hpp"

// sigma(s) = He(s)*epsilon(s) + Hk(s)*kappa(s)
struct DiscreteLayer
{
    std::vector<double> s;
    std::vector<std::array<double, 2>> He;
    std::vector<std::array<double, 2>> Hk;
};

struct DiscreteLimb
{
    // Nodes
    std::vector<double> s;
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> phi;

    // Section properties
    std::vector<double> hc;          // Total cross section height (used for contact)
    std::vector<double> Cee;
    std::vector<double> Ckk;
    std::vector<double> Cek;
    std::vector<double> rhoA;

    // Layer properties
    std::vector<DiscreteLayer> layers;

    DiscreteLimb(const InputData& input)
    {
        // 1. Nodes
        Curve nodes = ArcCurve::sample(input.profile_segments,
                                       input.profile_offset_x,
                                       input.profile_offset_y,
                                       input.profile_angle,
                                       input.settings_n_elements_limb);

        // Todo: Is there a more elegant way? Maybe have a Curve member?
        s = nodes.s;
        x = nodes.x;
        y = nodes.y;
        phi = nodes.phi;

        // 2. Sections
        Series w = CubicSpline::sample(input.sections_width, input.settings_n_elements_limb);
        Series h = CubicSpline::sample(input.sections_height, input.settings_n_elements_limb);

        int n_nodes = input.settings_n_elements_limb + 1;
        for(int i = 0; i < n_nodes; ++i)
        {
            double w_i = w.val(i);
            double h_i = h.val(i);

            double A = w_i*h_i;
            double I = A*h_i*h_i/3.0;

            hc.push_back(h_i);
            Cee.push_back(input.sections_E*A);
            Ckk.push_back(input.sections_E*I);
            Cek.push_back(input.sections_E*A*h_i/2.0);
            rhoA.push_back(input.sections_rho*A);
        }

        // 3. Layers

        // Todo
    }

    DiscreteLimb()
    {

    }

    /*
    std::vector<std::array<double, 2>> calculate_stresses(size_t layer, const std::vector<double>& epsilon, const std::vector<double>& kappa)
    {

    }
    */
};
