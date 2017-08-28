#pragma once
#include "model/InputData.hpp"
#include "numerics/ArcCurve.hpp"
#include "numerics/CubicSpline.hpp"
#include <vector>
#include <valarray>
#include <array>
#include <json.hpp>

using nlohmann::json;

// sigma_upper(s) = He_upper(s)*epsilon(s) + Hk_upper(s)*kappa(s)
// sigma_lower(s) = He_lower(s)*epsilon(s) + Hk_lower(s)*kappa(s)
struct LayerProperties
{
    double E;
    std::valarray<double> s;
    std::valarray<double> y_upper;
    std::valarray<double> y_lower;

    LayerProperties(size_t n)
        : s(n), y_upper(n), y_lower(n)
    {

    }

    std::valarray<double> sigma_upper(const std::valarray<double>& epsilon, const std::valarray<double>& kappa) const
    {
        return E*(y_upper*kappa - epsilon);    // Todo: Sign?
    }

    std::valarray<double> sigma_lower(const std::valarray<double>& epsilon, const std::valarray<double>& kappa) const
    {
        return E*(y_lower*kappa - epsilon);    // Todo: Sign?
    }
};

static void to_json(json& obj, const LayerProperties& val)
{
    obj["E"] = val.E;
    obj["s"] = val.s;
    obj["y_upper"] = val.y_upper;
    obj["y_lower"] = val.y_lower;
}

static void from_json(const json& obj, LayerProperties& val)
{
    val.E = obj["E"];
    val.s = obj["s"].get<std::valarray<double>>();
    val.y_upper = obj["y_upper"].get<std::valarray<double>>();
    val.y_lower = obj["y_lower"].get<std::valarray<double>>();
}

struct LimbProperties
{
    // Nodes
    std::valarray<double> s;
    std::valarray<double> x;
    std::valarray<double> y;
    std::valarray<double> w;
    std::valarray<double> h;
    std::valarray<double> phi;

    // Section properties
    std::valarray<double> hc;          // Total cross section height (used for contact)
    std::valarray<double> Cee;
    std::valarray<double> Ckk;
    std::valarray<double> Cek;
    std::valarray<double> rhoA;

    // Layer properties
    std::vector<LayerProperties> layers;

    LimbProperties(const InputData& input)
        : LimbProperties(input, input.settings_n_elements_limb)
    {

    }

    LimbProperties(const InputData& input, unsigned n_elements_limb)
        : s(n_elements_limb + 1),
          x(n_elements_limb + 1),
          y(n_elements_limb + 1),
          w(n_elements_limb + 1),
          h(n_elements_limb + 1),
          phi(n_elements_limb + 1),
          hc(n_elements_limb + 1),
          Cee(n_elements_limb + 1),
          Ckk(n_elements_limb + 1),
          Cek(n_elements_limb + 1),
          rhoA(n_elements_limb + 1)
    {
        // 1. Nodes
        Curve2D curve = ArcCurve::sample(input.profile_segments,
                                         input.profile_x0,
                                         input.profile_y0,
                                         input.profile_phi0,
                                         n_elements_limb);

        // Todo: Is there a more elegant way? Maybe have a Curve2D member?
        s = curve.s;
        x = curve.x;
        y = curve.y;
        phi = curve.phi;

        // 2. Sections
        Series width = CubicSpline::sample(input.sections_width, n_elements_limb);
        Series height = CubicSpline::sample(input.sections_height, n_elements_limb);

        for(size_t i = 0; i < s.size(); ++i)
        {
            double w_i = width.val(i);
            double h_i = height.val(i);

            double A = w_i*h_i;
            double I = A*h_i*h_i/3.0;

            w[i] = w_i;
            h[i] = h_i;

            hc[i] = h_i;
            Cee[i] = input.sections_E*A;
            Ckk[i] = input.sections_E*I;
            Cek[i] = input.sections_E*A*h_i/2.0;
            rhoA[i] = input.sections_rho*A;
        }

        // 3. Layers

        layers.push_back({n_elements_limb + 1});
        layers[0].E = input.sections_E;

        for(size_t i = 0; i < s.size(); ++i)
        {
            layers[0].s[i] = s[i];
            layers[0].y_upper[i] = 0.0;
            layers[0].y_lower[i] = -height.val(i);
        }
    }

    LimbProperties()
    {

    }
};

static void to_json(json& obj, const LimbProperties& val)
{
    obj["s"] = val.s;
    obj["x"] = val.x;
    obj["y"] = val.y;
    obj["w"] = val.w;
    obj["h"] = val.h;
    obj["phi"] = val.phi;
    obj["hc"] = val.hc;
    obj["Cee"] = val.Cee;
    obj["Ckk"] = val.Ckk;
    obj["Cek"] = val.Cek;
    obj["rhoA"] = val.rhoA;
}

static void from_json(const json& obj, LimbProperties& val)
{
    val.s = obj["s"].get<std::valarray<double>>();
    val.x = obj["x"].get<std::valarray<double>>();
    val.y = obj["y"].get<std::valarray<double>>();
    val.w = obj["w"].get<std::valarray<double>>();
    val.h = obj["h"].get<std::valarray<double>>();
    val.phi = obj["phi"].get<std::valarray<double>>();;
    val.hc = obj["hc"].get<std::valarray<double>>();
    val.Cee = obj["Cee"].get<std::valarray<double>>();
    val.Ckk = obj["Ckk"].get<std::valarray<double>>();
    val.Cek = obj["Cek"].get<std::valarray<double>>();
    val.rhoA = obj["rhoA"].get<std::valarray<double>>();
}
