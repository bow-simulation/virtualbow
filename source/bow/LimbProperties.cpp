#include "LimbProperties.hpp"
#include "bow/input/InputData.hpp"
#include "numerics/ArcCurve.hpp"
#include "numerics/CubicSpline.hpp"

LimbProperties::LimbProperties(const InputData& input)
    : LimbProperties(input, input.settings.n_elements_limb)
{

}

LimbProperties::LimbProperties(const InputData& input, unsigned n_elements_limb)
    : s(VectorXd::Zero(n_elements_limb + 1)),
      x(VectorXd::Zero(n_elements_limb + 1)),
      y(VectorXd::Zero(n_elements_limb + 1)),
      phi(VectorXd::Zero(n_elements_limb + 1)),
      w(VectorXd::Zero(n_elements_limb + 1)),
      h(VectorXd::Zero(n_elements_limb + 1)),
      Cee(VectorXd::Zero(n_elements_limb + 1)),
      Ckk(VectorXd::Zero(n_elements_limb + 1)),
      Cek(VectorXd::Zero(n_elements_limb + 1)),
      rhoA(VectorXd::Zero(n_elements_limb + 1))
{
    // 1. Nodes
    Curve2D curve = ArcCurve::sample(input.profile.segments,
                                     input.profile.x0,
                                     input.profile.y0,
                                     input.profile.phi0,
                                     n_elements_limb);

    // Todo: Is there a more elegant way? Maybe have a Curve2D member? C++17 structured bindings?
    s = curve.s;
    x = curve.x;
    y = curve.y;
    phi = curve.phi;

    // 2. Sections properties
    Series width = CubicSpline::sample(input.width, n_elements_limb);

    std::vector<Series> heights;
    for(auto& layer: input.layers)
        heights.push_back(CubicSpline::sample(layer.height, n_elements_limb));

    // i: Limb index
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

            Cee[i] += input.layers[j].E*A;
            Ckk[i] += input.layers[j].E*I;
            Cek[i] -= input.layers[j].E*A*y;
            rhoA[i] += input.layers[j].rho*A;

            y_bottom += h;
        }
    }

    /*
    // 3. Layer properties

    layers.push_back({n_elements_limb + 1});
    layers[0].E = input.layers[0].E;

    for(size_t i = 0; i < s.size(); ++i)
    {
        layers[0].s[i] = s[i];
        layers[0].y_back[i] = 0.5*height.val(i);
        layers[0].y_belly[i] = -0.5*height.val(i);
    }
    */
}

LimbProperties::LimbProperties()
{

}
