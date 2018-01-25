#include "LimbProperties.hpp"
#include "bow/input/InputData.hpp"

LayerProperties::LayerProperties(size_t n)
    : s(n), y_back(n), y_belly(n)
{

}

VectorXd LayerProperties::sigma_back(const VectorXd& epsilon, const VectorXd& kappa) const
{
    return E*(epsilon - y_back.cwiseProduct(kappa));
}

VectorXd LayerProperties::sigma_belly(const VectorXd& epsilon, const VectorXd& kappa) const
{
    return E*(epsilon - y_belly.cwiseProduct(kappa));
}



LimbProperties::LimbProperties(const InputData& input)
    : LimbProperties(input, input.settings.n_elements_limb)
{

}

LimbProperties::LimbProperties(const InputData& input, unsigned n_elements_limb)
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
    Curve2D curve = ArcCurve::sample(input.profile.segments,
                                     input.profile.x0,
                                     input.profile.y0,
                                     input.profile.phi0,
                                     n_elements_limb);

    // Todo: Is there a more elegant way? Maybe have a Curve2D member?
    s = curve.s;
    x = curve.x;
    y = curve.y;
    phi = curve.phi;

    // 2. Sections
    Series width = CubicSpline::sample(input.width, n_elements_limb);
    Series height = CubicSpline::sample(input.layers[0].height, n_elements_limb);

    for(size_t i = 0; i < s.size(); ++i)
    {
        double w_i = width.val(i);
        double h_i = height.val(i);

        double A = w_i*h_i;
        double I = A*h_i*h_i/3.0;

        w[i] = w_i;
        h[i] = h_i;

        hc[i] = h_i;
        Cee[i] = input.layers[0].E*A;
        Ckk[i] = input.layers[0].E*I;
        Cek[i] = input.layers[0].E*A*h_i/2.0;
        rhoA[i] = input.layers[0].rho*A;
    }

    // 3. Layers

    layers.push_back({n_elements_limb + 1});
    layers[0].E = input.layers[0].E;

    for(size_t i = 0; i < s.size(); ++i)
    {
        layers[0].s[i] = s[i];
        layers[0].y_back[i] = 0.0;
        layers[0].y_belly[i] = -height.val(i);
    }
}

LimbProperties::LimbProperties()
{

}
