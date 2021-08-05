#pragma once
#include "solver/model/input/InputData.hpp"
#include "solver/model/ProfileCurve.hpp"
#include "solver/numerics/CubicSpline.hpp"

#include <boost/container/small_vector.hpp>
using boost::container::small_vector;

class ContinuousLimb;

class ContinuousLayer
{
public:
    ContinuousLayer(const ContinuousLimb& limb, const Layer& layer);

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
    static constexpr int n = 15;
    ContinuousLimb(const InputData& input);

    const std::vector<ContinuousLayer>& get_layers() const;

    double s_min() const;    // Arc length at start of the limb
    double s_max() const;    // Arc length at end of the limb

    double get_p(double s) const;    // Calculates the relative position p in [0, 1] from arc length s
    double get_s(double p) const;    // Calculates the arc length from a relative position p in [0, 1]
    double get_w(double s) const;    // Returns the limb's width at the specified arc length
    double get_h(double s) const;    // Calculates the total height of the section (sum of all layers)

    small_vector<double, n> get_y(double s) const;    // Calculates the coordinates of all layer transitions from back to belly
    CurvePoint get_r(double s) const;                  // Returns the position and orientation of the profile curve (x, y, phi)
    Matrix<2, 2> get_C(double s) const;               // Calculates the section's stiffness parameters Cee, Ckk, Cek
    double get_rhoA(double s) const;                  // Calculates the section's linear density

private:
    ProfileCurve profile;
    CubicSpline width;
    std::vector<ContinuousLayer> layers;
};
