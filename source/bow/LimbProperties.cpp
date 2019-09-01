#include "LimbProperties.hpp"
#include "bow/ContinuousLimb.hpp"
#include "numerics/Linspace.hpp"

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
      rhoA(VectorXd::Zero(n)),
      layers(input.layers.size(), LayerProperties(n, n))
{
    ContinuousLimb limb(input);

    std::vector<double> s = linspace(limb.s_min(), limb.s_max(), n);
    for(size_t i = 0; i < n; ++i)
    {
        // Profile
        Vector<3> r = limb.get_r(s[i]);
        length[i] = s[i];
        x_pos[i] = r[0];
        y_pos[i] = r[1];
        angle[i] = r[2];

        // Geometry
        width[i] = limb.get_w(s[i]);
        height[i] = limb.get_h(s[i]);

        // Section properties
        rhoA[i] = limb.get_rhoA(s[i]);
        Matrix<2, 2> C = limb.get_C(s[i]);
        Cee[i] = C(0, 0);
        Ckk[i] = C(1, 1);
        Cek[i] = C(0, 1);

        // Layer properties
        auto y = limb.get_y(s[i]);
        for(size_t j = 0; j < input.layers.size(); ++j)
        {
            layers[j].length(i) = s[i];

            layers[j].y_back(i) = y[j];
            layers[j].y_belly(i) = y[j+1];

            // Todo: Add method to ContinuousLimb to calculate those
            layers[j].He_back(i, i) =  input.layers[j].E;
            layers[j].He_belly(i, i) = input.layers[j].E;

            layers[j].Hk_back(i, i) = -input.layers[j].E*layers[j].y_back(i);
            layers[j].Hk_belly(i, i) = -input.layers[j].E*layers[j].y_belly(i);
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
