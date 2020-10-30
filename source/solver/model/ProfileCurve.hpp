#pragma once
#include "solver/numerics/Eigen.hpp"
#include "solver/numerics/LinearSpline.hpp"
#include <optional>
#include <vector>
#include <cmath>
#include <string>

struct Point {
    double x;
    double y;
    double phi;
};

struct SegmentInput {
    std::optional<double> length;
    std::optional<double> angle;
    std::optional<double> delta_x;
    std::optional<double> delta_y;

    std::optional<std::string> validate() const {
        if(length.has_value()) {
            if(*length <= 0.0) {
                return std::string("Length must be > 0");
            }
            if(delta_x.has_value() && (*length < *delta_x)) {
                return std::string("Length must be >= delta x");
            }
            if(delta_y.has_value() && (*length < *delta_y)) {
                return std::string("Length must be >= delta y");
            }
        }
        return std::nullopt;
    }

    int dimension() const {
        return length.has_value() + angle.has_value() + delta_x.has_value() + delta_y.has_value();
    }
};

class Segment {
public:
    Segment(const std::vector<double>& c);

    static std::vector<double> estimate_coeffs(const Point& point, const SegmentInput& input);
    const std::vector<double>& get_coeffs() const;
    void set_coeffs(const std::vector<double>&  c);

    double length() const;
    double angle() const;
    double energy() const;

    double phi(double t) const;    // Orientation angle in [-PI, PI]
    double x(double t) const;
    double y(double t) const;

    double dxdt(double t) const;
    double dydt(double t) const;

    double dxdt2(double t) const;
    double dydt2(double t) const;

    Vector<8> grad_length() const;
    Vector<8> grad_angle() const;
    Vector<8> grad_energy() const;

    Vector<8> grad_phi(double t) const;
    Vector<8> grad_x(double t) const;
    Vector<8> grad_y(double t) const;

    Vector<8> grad_dxdt(double t) const;
    Vector<8> grad_dydt(double t) const;

    Vector<8> grad_dxdt2(double t) const;
    Vector<8> grad_dydt2(double t) const;

private:
    std::vector<double> c;
};

class ProfileCurve {
public:
    static std::vector<SegmentInput> input_from_matrix(const MatrixXd& matrix);
    static std::optional<std::string> validate_input(const std::vector<SegmentInput>& input);

    ProfileCurve(const std::vector<SegmentInput>& input);

    const std::vector<Segment>& get_segments() const;
    const std::vector<Point>& get_nodes() const;
    double length() const;
    Point operator()(double s) const;

private:
    std::vector<Segment> segments;
    std::vector<Point> points;
    LinearSpline transform;

    static Segment optimize_local(const Point& point, const SegmentInput& input);
};
