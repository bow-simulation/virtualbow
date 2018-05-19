#include "LimbProperties.hpp"
#include "numerics/ArcCurve.hpp"
#include "numerics/CubicSpline.hpp"

LimbProperties::LimbProperties(const InputData& input)
    : LimbProperties(input, input.settings.n_limb_elements + 1)
{

}

LimbProperties::LimbProperties(const InputData& input, unsigned n)
    : length(VectorXd::Zero(n)),
      angle(VectorXd::Zero(n)),
      x_pos(VectorXd::Zero(n)),
      y_pos(VectorXd::Zero(n)),
      width(VectorXd::Zero(n)),
      height(VectorXd::Zero(n)),
      Cee(VectorXd::Zero(n)),
      Ckk(VectorXd::Zero(n)),
      Cek(VectorXd::Zero(n)),
      rhoA(VectorXd::Zero(n))
{
    // 1. Nodes
    Curve2D curve = ArcCurve::sample(input.profile,
                                     input.dimensions.handle_length/2.0,
                                     input.dimensions.handle_setback,
                                     input.dimensions.handle_angle,
                                     n-1);

    // Todo: Is there a more elegant way? Maybe have a Curve2D member? C++17 structured bindings?
    length = curve.s;
    angle = curve.phi;
    x_pos = curve.x;
    y_pos = curve.y;

    // 2. Sections properties
    Series width = CubicSpline(input.width).sample(n-1);

    std::vector<Series> heights;
    for(auto& layer: input.layers)
    {
        heights.push_back(CubicSpline(layer.height).sample(n-1));
        layers.push_back(LayerProperties(n, n));
    }

    // i: Section index
    for(size_t i = 0; i < length.size(); ++i)
    {
        // Width
        this->width[i] = width.val(i);

        // Total height
        for(auto& height: heights)
            this->height[i] += height.val(i);

        // j: Layer index
        double y_top = 0.5*this->height[i];
        for(size_t j = 0; j < input.layers.size(); ++j)
        {
            double h = heights[j].val(i);
            double y = y_top - 0.5*h;
            double A = this->width[i]*h;
            double I = A*(h*h/12.0 + y*y);

            // Limb properties

            Cee[i] += input.layers[j].E*A;
            Ckk[i] += input.layers[j].E*I;
            Cek[i] -= input.layers[j].E*A*y;
            rhoA[i] += input.layers[j].rho*A;

            // Layer properties

            layers[j].length(i) = length[i];

            layers[j].y_back(i) = y + 0.5*h;
            layers[j].y_belly(i) = y - 0.5*h;

            layers[j].He_back(i, i) =  input.layers[j].E;
            layers[j].He_belly(i, i) = input.layers[j].E;

            layers[j].Hk_back(i, i) = -input.layers[j].E*layers[j].y_back(i);
            layers[j].Hk_belly(i, i) = -input.layers[j].E*layers[j].y_belly(i);

            // Next layer

            y_top -= h;
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
