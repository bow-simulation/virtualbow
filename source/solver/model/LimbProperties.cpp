#include "LimbProperties.hpp"
#include "solver/model/input/InputData.hpp"
#include "solver/model/ContinuousLimb.hpp"
#include "solver/numerics/Linspace.hpp"

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
      rhoA(VectorXd::Zero(n)),
      Cee(VectorXd::Zero(n)),
      Ckk(VectorXd::Zero(n)),
      Cek(VectorXd::Zero(n)),
      m(n-1),
      layers(input.layers.size(), LayerProperties(n, n))
{
    ContinuousLimb limb(input);

    std::vector<double> s = linspace(0.0, limb.length(), n);
    for(size_t i = 0; i < n; ++i) {
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
        for(size_t j = 0; j < input.layers.size(); ++j) {
            layers[j].length(i) = s[i];

            // Todo: Add method to ContinuousLimb to calculate those
            auto& material = input.materials.at(input.layers[j].material);

            layers[j].He_back(i, i) =  material.E;
            layers[j].He_belly(i, i) = material.E;

            layers[j].Hk_back(i, i) = -material.E*y[j];
            layers[j].Hk_belly(i, i) = -material.E*y[j+1];
        }
    }

    // Calculate section masses
    for(int i = 0; i < n-1; ++i) {
        m[i] = 0.5*(rhoA(i+1) + rhoA(i))*(length(i+1) - length(i));
    }

    // More layer properties
    for(size_t i = 0; i < input.layers.size(); ++i) {
        auto& material = input.materials.at(input.layers[i].material);

        layers[i].name = input.layers[i].name;
        layers[i].color = material.color;
        layers[i].rho = material.rho;
        layers[i].E = material.E;
    }

}
