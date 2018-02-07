#include "LimbProperties.hpp"
#include "bow/input/InputData.hpp"
#include "numerics/ArcCurve.hpp"
#include "numerics/CubicSpline.hpp"

#include <QtCore>
#include <iostream>

LimbProperties::LimbProperties(const InputData& input)
    : LimbProperties(input, input.settings.n_elements_limb + 1)
{

}

LimbProperties::LimbProperties(const InputData& input, unsigned n)
    : s(VectorXd::Zero(n)),
      x(VectorXd::Zero(n)),
      y(VectorXd::Zero(n)),
      phi(VectorXd::Zero(n)),
      w(VectorXd::Zero(n)),
      h(VectorXd::Zero(n)),
      Cee(VectorXd::Zero(n)),
      Ckk(VectorXd::Zero(n)),
      Cek(VectorXd::Zero(n)),
      rhoA(VectorXd::Zero(n))
{
    // 1. Nodes
    Curve2D curve = ArcCurve::sample(input.profile.segments,
                                     input.profile.x0,
                                     input.profile.y0,
                                     input.profile.phi0,
                                     n-1);

    // Todo: Is there a more elegant way? Maybe have a Curve2D member? C++17 structured bindings?
    s = curve.s;
    x = curve.x;
    y = curve.y;
    phi = curve.phi;

    // 2. Sections properties
    Series width = CubicSpline::sample(input.width, n-1);

    std::vector<Series> heights;
    for(auto& layer: input.layers)
    {
        heights.push_back(CubicSpline::sample(layer.height, n-1));
        layers.push_back(LayerProperties(n, n));
    }

    // i: Section index
    for(size_t i = 0; i < s.size(); ++i)
    {
        // Width
        w[i] = width.val(i);

        // Total height
        for(auto& height: heights)
            h[i] += height.val(i);

        // j: Layer index
        double y_bottom = -0.5*h[i];
        for(size_t j = 0; j < input.layers.size(); ++j)
        {
            double h = heights[j].val(i);
            double y = y_bottom + 0.5*h;
            double A = w[i]*h;
            double I = A*(h*h/12.0 + y*y);

            // Limb properties

            Cee[i] += input.layers[j].E*A;
            Ckk[i] += input.layers[j].E*I;
            Cek[i] -= input.layers[j].E*A*y;
            rhoA[i] += input.layers[j].rho*A;

            // Layer properties

            layers[j].s(i) = s[i];

            layers[j].y_back(i) = y + 0.5*h;
            layers[j].y_belly(i) = y - 0.5*h;

            layers[j].He_back(i, i) =  input.layers[j].E;
            layers[j].He_belly(i, i) = input.layers[j].E;

            layers[j].Hk_back(i, i) = -input.layers[j].E*layers[j].y_back(i);
            layers[j].Hk_belly(i, i) = -input.layers[j].E*layers[j].y_belly(i);

            // Next layer

            y_bottom += h;
        }
    }

    // 3. Layer properties
    /*
    LayerProperties layer(n, n);
    for(size_t i = 0; i < n; ++i)
    {
        layer.s(i) = s(i);

        layer.y_back(i) = 0.5*heights[0].val(i);
        layer.y_belly(i) = -0.5*heights[0].val(i);

        layer.He_back(i, i) =  input.layers[0].E;
        layer.He_belly(i, i) = input.layers[0].E;

        layer.Hk_back(i, i) = input.layers[0].E*layer.y_back(i);
        layer.Hk_belly(i, i) = input.layers[0].E*layer.y_belly(i);
    }

    layers.push_back(layer);
    */
}

LimbProperties::LimbProperties()
{

}
