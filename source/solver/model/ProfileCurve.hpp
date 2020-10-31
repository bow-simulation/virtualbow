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

    std::optional<std::string> validate() const;
    int dimension() const;
};

class Segment {
public:
    Segment(const Vector<8>& c);

    static Vector<8> estimate_coeffs(const Point& point, const SegmentInput& input);
    static int n_coeffs();

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
    Vector<8> c;
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

    static Vector<8> optimize_segment(const Point& point, const SegmentInput& input);
    static std::vector<Vector<8>> optimize_local(const Point& startpoint, const std::vector<SegmentInput>& input);
    static std::vector<Vector<8>> optimize_global(const std::vector<Vector<8>>& coeffs, const Point& startpoint, const std::vector<SegmentInput>& input);

    struct ConstraintData1 {
        size_t offset;    // Offset of the segment's coefficients
        double value;     // Value of the constraint
    };

    struct ConstraintData2 {
        size_t offset0;    // Offset of the first segment's coefficients
        size_t offset1;    // Offset of the second segment's coefficients
    };

    static double objective(const std::vector<double>& x, std::vector<double>& grad, void* data) noexcept;

    static double constraint_phi0(const std::vector<double>& x, std::vector<double>& grad, void *data) noexcept;
    static double constraint_x0(const std::vector<double>& x, std::vector<double>& grad, void *data) noexcept;
    static double constraint_y0(const std::vector<double>& x, std::vector<double>& grad, void *data) noexcept;

    static double constraint_length(const std::vector<double>& x, std::vector<double>& grad, void *data) noexcept;
    static double constraint_angle(const std::vector<double>& x, std::vector<double>& grad, void *data) noexcept;
    static double constraint_delta_x(const std::vector<double>& x, std::vector<double>& grad, void *data) noexcept;
    static double constraint_delta_y(const std::vector<double>& x, std::vector<double>& grad, void *data) noexcept;

    static double constraint_x(const std::vector<double>& x, std::vector<double>& grad, void *data) noexcept;
    static double constraint_y(const std::vector<double>& x, std::vector<double>& grad, void *data) noexcept;
    static double constraint_dxdt(const std::vector<double>& x, std::vector<double>& grad, void *data) noexcept;
    static double constraint_dydt(const std::vector<double>& x, std::vector<double>& grad, void *data) noexcept;
    static double constraint_dxdt2(const std::vector<double>& x, std::vector<double>& grad, void *data) noexcept;
    static double constraint_dydt2(const std::vector<double>& x, std::vector<double>& grad, void *data) noexcept;

    static void copy_to_vec(const Vector<8>& source, std::vector<double>& target, size_t offset);
};
