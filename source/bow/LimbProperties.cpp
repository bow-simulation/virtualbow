#include "LimbProperties.hpp"
#include "bow/input/InputData.hpp"
#include "numerics/ArcCurve.hpp"
#include "numerics/CubicSpline.hpp"

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
        heights.push_back(CubicSpline::sample(layer.height, n-1));

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

    // 3. Layer properties

    LayerProperties layer;

    layer.s = VectorXd::Zero(n);
    layer.y_back = VectorXd::Zero(n);
    layer.y_belly = VectorXd::Zero(n);
    layer.He_back = MatrixXd::Zero(n, n);
    layer.He_belly = MatrixXd::Zero(n, n);
    layer.Hk_back = MatrixXd::Zero(n, n);
    layer.Hk_belly = MatrixXd::Zero(n, n);

    for(size_t i = 0; i < n; ++i)
    {
        layer.s(i) = s(i);

        layer.He_back(i, i) =  input.layers[0].E;
        layer.Hk_back(i, i) = -input.layers[0].E*0.5*heights[0].val(i);

        layer.He_belly(i, i) = input.layers[0].E;
        layer.Hk_belly(i, i) = input.layers[0].E*0.5*heights[0].val(i);
    }

    layers.push_back(layer);
}

LimbProperties::LimbProperties()
{

}
