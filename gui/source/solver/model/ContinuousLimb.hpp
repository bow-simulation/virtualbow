#pragma once
#include "solver/model/input/InputData.hpp"
#include "solver/model/profile/ProfileCurve.hpp"
#include "solver/numerics/CubicSpline.hpp"

class ContinuousLimb;

class ContinuousLayer
{
public:
    ContinuousLayer(const ContinuousLimb& limb, const Layer& layer, const Material& material);

    double s_min() const;
    double s_max() const;

    double get_h(double s) const;
    double get_rho() const;
    double get_E() const;

private:
    const ContinuousLimb& limb;
    CubicSpline height;
    double rho;
    double E;
};

class ContinuousLimb
{
public:
    ContinuousLimb(const InputData& input);

    const std::vector<ContinuousLayer>& get_layers() const;

    double length() const;    // Arc length of the limb

    // TODO: Remove since s = p*length now?
    double get_p(double s) const;    // Calculates the relative position p in [0, 1] from arc length s
    double get_s(double p) const;    // Calculates the arc length from a relative position p in [0, 1]

    Vector<3> get_r(double s) const; // Returns the position and orientation of the limb centerline (x, y, phi)
    double get_w(double s) const;    // Returns the limb's width at the specified arc length
    double get_h(double s) const;    // Calculates the total height of the section (sum of all layers)

    std::vector<double> get_y(double s) const;    // Calculates the coordinates of all layer transitions from back to belly
    Matrix<2> get_C(double s) const;               // Calculates the section's stiffness parameters Cee, Ckk, Cek
    double get_rhoA(double s) const;                  // Calculates the section's linear density

private:
    ProfileCurve profile;
    CubicSpline width;
    std::vector<ContinuousLayer> layers;
    Vector<2> translation;
    double rotation;
};
