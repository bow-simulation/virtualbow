#include "ContinuousLimb.hpp"

ContinuousLayer::ContinuousLayer(const ContinuousLimb& limb, const Layer& layer)
    : limb(limb),
      height(layer.height),
      rho(layer.rho),
      E(layer.E)
{

}

double ContinuousLayer::s_min() const
{
    return limb.get_s(height.arg_min());
}

double ContinuousLayer::s_max() const
{
    return limb.get_s(height.arg_max());
}

double ContinuousLayer::get_h(double s) const
{
    return height(limb.get_p(s), 0.0);  // Zero if out of bounds
}

double ContinuousLayer::get_rho() const
{
    return rho;
}

double ContinuousLayer::get_E() const
{
    return E;
}

ContinuousLimb::ContinuousLimb(const InputData& input)
    : profile(input.profile),
      width(input.width),
      translation{0.5*input.dimensions.handle_length, input.dimensions.handle_setback},
      rotation(input.dimensions.handle_angle)
{
    for(const Layer& layer: input.layers) {
        layers.push_back(ContinuousLayer(*this, layer));
    }
}

const std::vector<ContinuousLayer>& ContinuousLimb::get_layers() const
{
    return layers;
}

double ContinuousLimb::length() const
{
    return profile.length();
}

double ContinuousLimb::get_p(double s) const
{
    return s/length();
}

double ContinuousLimb::get_s(double p) const
{
    return p*length();
}

Vector<3> ContinuousLimb::get_r(double s) const
{
    Vector<2> position = profile.position(s);
    return {
        translation[0] + cos(rotation)*position[0] - sin(rotation)*position[1],
        translation[1] + sin(rotation)*position[0] + cos(rotation)*position[1],
        rotation + profile.angle(s)
    };
}

double ContinuousLimb::get_w(double s) const
{
    return width(get_p(s));
}

double ContinuousLimb::get_h(double s) const
{
    double h = 0.0;
    for(auto& layer: layers)
    {
        h += layer.get_h(s);
    }

    return h;
}

small_vector<double, ContinuousLimb::n> ContinuousLimb::get_y(double s) const
{
    small_vector<double, n> y(layers.size() + 1);
    for(size_t i = 0; i < layers.size(); ++i) {
        y[i+1] = y[i] - layers[i].get_h(s);
    }

    return y;
}

// Calculates the section's stiffness parameters Cee, Ckk, Cek
Matrix<2, 2> ContinuousLimb::get_C(double s) const
{
    double Cee = 0.0;
    double Ckk = 0.0;
    double Cek = 0.0;

    small_vector<double, n> y_pos = get_y(s);
    for(size_t i = 0; i < layers.size(); ++i)
    {
        double h = layers[i].get_h(s);
        double A = width(s)*h;
        double y = 0.5*(y_pos[i] + y_pos[i+1]);
        double I = A*(h*h/12.0 + y*y);

        Cee += layers[i].get_E()*A;
        Ckk += layers[i].get_E()*I;
        Cek -= layers[i].get_E()*A*y;
    }

    return (Matrix<2, 2>() << Cee, Cek, Cek, Ckk).finished();
}

double ContinuousLimb::get_rhoA(double s) const
{
    double w = get_w(s);
    double rhoA = 0.0;
    for(auto& layer: layers)
    {
        rhoA += layer.get_rho()*w*layer.get_h(s);
    }

    return rhoA;
}

