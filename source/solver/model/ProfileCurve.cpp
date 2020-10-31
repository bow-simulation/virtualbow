#include "ProfileCurve.hpp"
#include "solver/numerics/Integration.hpp"
#include "solver/numerics/Linspace.hpp"
#include "solver/numerics/Eigen.hpp"
#include "solver/numerics/Utils.hpp"
#include <nlopt.hpp>
#include <list>

std::optional<std::string> SegmentInput::validate() const {
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

int SegmentInput::dimension() const {
    return length.has_value() + angle.has_value() + delta_x.has_value() + delta_y.has_value();
}

Segment::Segment(const Vector<8>& c)
    : c(c) {

}

Vector<8> Segment::estimate_coeffs(const Point& point, const SegmentInput& input) {
    // Returns coefficients from four Bezier points
    // https://de.wikipedia.org/wiki/B%C3%A9zierkurve
    auto from_bezier_points4 = [](Vector<2> p0, Vector<2> p1, Vector<2> p2, Vector<2> p3) -> Vector<8> {
        Vector<2> a0 = p0;
        Vector<2> a1 = -3.0*p0 + 3.0*p1;
        Vector<2> a2 = 3.0*p0 - 6.0*p1 + 3.0*p2;
        Vector<2> a3 = -p0 + 3.0*p1 - 3.0*p2 + p3;

        return { a0[0], a1[0], a2[0], a3[0], a0[1], a1[1], a2[1], a3[1] };
    };

    // Returns coefficients from two Bezier points
    auto from_bezier_points3 = [](Vector<2> p0, Vector<2> p1, Vector<2> p2) -> Vector<8> {
        Vector<2> a0 = p0;
        Vector<2> a1 = -2.0*p0 + 2.0*p1;
        Vector<2> a2 = p0 - 2*p1 + p2;

        return { a0[0], a1[0], a2[0], 0, a0[1], a1[1], a2[1], 0 };
    };

    // Returns coefficients for a curve that interpolates four points
    auto from_interpolation = [](Vector<2> p0, Vector<2> p1, Vector<2> p2, Vector<2> p3) -> Vector<8> {
        Matrix<8, 8> A{
            { 1.0,     0.0,     0.0,      0.0, 0.0,     0.0,     0.0,      0.0 },
            { 0.0,     0.0,     0.0,      0.0, 1.0,     0.0,     0.0,      0.0 },
            { 1.0, 1.0/3.0, 1.0/9.0, 1.0/27.0, 0.0,     0.0,     0.0,      0.0 },
            { 0.0,     0.0,     0.0,      0.0, 1.0, 1.0/3.0, 1.0/9.0, 1.0/27.0 },
            { 1.0, 2.0/3.0, 4.0/9.0, 8.0/27.0, 0.0,     0.0,     0.0,      0.0 },
            { 0.0,     0.0,     0.0,      0.0, 1.0, 2.0/3.0, 4.0/9.0, 8.0/27.0 },
            { 1.0,     1.0,     1.0,      1.0, 0.0,     0.0,     0.0,      0.0 },
            { 0.0,     0.0,     0.0,      0.0, 1.0,     1.0,     1.0,      1.0 }
        };

        Vector<8> p;
        p << p0, p1, p2, p3;

        return A.householderQr().solve(p);
    };

    auto case_a_x_y = [&](double angle, double delta_x, double delta_y) {
        Vector<2> p0 = { point.x, point.y };
        Vector<2> p1 = p0 + Vector<2>{ delta_x, delta_y };
        double l = 1.0/3.0*hypot(p1[0] - p0[0], p1[1] - p0[1]);

        Vector<2> b0 = p0;
        Vector<2> b1 = p0 + l*Vector<2>{ cos(point.phi), sin(point.phi) };
        Vector<2> b2 = p1 - l*Vector<2>{ cos(point.phi + angle), sin(point.phi + angle) };
        Vector<2> b3 = p1;

        return from_bezier_points4(b0, b1, b2, b3);
    };

    auto case_l_a = [&](double length, double angle) {
        auto get_circle_point = [](Point point, double k, double s) -> Vector<2> {
            if(k != 0.0) {
                return {
                    point.x + 1/k*(sin(point.phi + k*s) - sin(point.phi)),
                    point.y - 1/k*(cos(point.phi + k*s) - cos(point.phi))
                };
            }
            else {
                return {
                    point.x + s*cos(point.phi),
                    point.y + s*sin(point.phi)
                };
            }
        };

        double k = angle/length;
        Vector<2> p0 = get_circle_point(point, k, 0);
        Vector<2>p1 = get_circle_point(point, k, 1.0/3.0*length);
        Vector<2>p2 = get_circle_point(point, k, 2.0/3.0*length);
        Vector<2>p3 = get_circle_point(point, k, length);

        return from_interpolation(p0, p1, p2, p3);
    };

    auto case_l_x = [&](double length, double delta_x) {
        if(length < delta_x) {
            throw std::runtime_error("Impossible constraint, length must be greater than delta_x.");
        }

        double arg = 2.0*delta_x/length - cos(point.phi);
        double alpha, a;

        if(arg >= -1.0 && arg <= 1.0) {
            alpha = sgn(point.phi)*acos(2*delta_x/length - cos(point.phi));
            a = length/2;
        }
        else {
            alpha = 0.0;
            a = (length - delta_x)/(1 - cos(point.phi));
        }

        Vector<2> p0 = { point.x, point.y };
        Vector<2> p1 = p0 + a*Vector<2>{ cos(point.phi), sin(point.phi) };
        Vector<2> p2 = p1 + (length - a)*Vector<2>{ cos(alpha), sin(alpha) };

        return from_bezier_points3(p0, p1, p2);
    };


    auto case_l_y = [&](double length, double delta_y) {
        if(length < delta_y) {
            throw std::runtime_error("Impossible constraint, length must be greater than delta_y.");
        }

        double arg = 2.0*delta_y/length - sin(point.phi);
        double alpha, a;

        if(arg >= -1.0 && arg <= 1.0) {
            alpha = sgn(M_PI_2 - abs(point.phi))*acos(2*delta_y/length - sin(point.phi));
            a = length/2.0;
        }
        else {
            alpha = 0.0;
            a = (length - delta_y)/(1 - sin(point.phi));
        }

        Vector<2> p0 = { point.x, point.y };
        Vector<2> p1 = p0 + a*Vector<2>{ cos(point.phi), sin(point.phi) };
        Vector<2> p2 = p1 + (length - a)*Vector<2>{ sin(alpha), cos(alpha) };

        return from_bezier_points3(p0, p1, p2);
    };

    auto case_a_x = [&](double angle, double delta_x) {
        double den = cos(point.phi) + cos(point.phi + angle/2) + cos(point.phi + angle);
        if(den == 0.0) {
            return case_a_x_y(angle, delta_x, 0.0);
        }

        double a = delta_x/den;
        if(a < 0.0) {
            return case_a_x_y(angle, delta_x, 0);
        }

        Vector<2> p0 = Vector<2>{ point.x, point.y };
        Vector<2> p1 = p0 + a*Vector<2>{ cos(point.phi), sin(point.phi) };
        Vector<2> p2 = p1 + a*Vector<2>{ cos(point.phi + angle/2), sin(point.phi + angle/2) };
        Vector<2> p3 = p2 + a*Vector<2>{ cos(point.phi + angle), sin(point.phi + angle) };

        return from_bezier_points4(p0, p1, p2, p3);
    };

    auto case_a_y = [&](double angle, double delta_y) {
        double den = sin(point.phi) + sin(point.phi + angle/2) + sin(point.phi + angle);
        if(den == 0.0) {
            return case_a_x_y(angle, 0, delta_y);
        }

        double a = delta_y/den;
        if(a < 0.0) {
            return case_a_x_y(angle, 0, delta_y);
        }

        Vector<2> p0 = { point.x, point.y };
        Vector<2> p1 = p0 + a*Vector<2>{ cos(point.phi), sin(point.phi) };
        Vector<2> p2 = p1 + a*Vector<2>{ cos(point.phi + angle/2), sin(point.phi + angle/2) };
        Vector<2> p3 = p2 + a*Vector<2>{ cos(point.phi + angle), sin(point.phi + angle) };

        return from_bezier_points4(p0, p1, p2, p3);
    };

    auto case_x_y = [&](double delta_x, double delta_y) {
        double a = atan2(delta_y, delta_x);
        return case_a_x_y(2*a, delta_x, delta_y);
    };

    auto case_l_a_x_y = [&](double length, double angle, double delta_x, double delta_y) {
        return case_a_x_y(angle, delta_x, delta_y);
    };

    auto case_l_a_x = [&](double length, double angle, double delta_x) {
        return case_a_x(angle, delta_x);
    };

    auto case_l_a_y = [&](double length, double angle, double delta_y) {
        return case_a_y(angle, delta_y);
    };

    auto case_l_x_y = [&](double length, double delta_x, double delta_y) {
        return case_x_y(delta_x, delta_y);
    };

    // Case 1: {length, angle, delta_x, delta_y}
    if(input.length and input.angle and input.delta_x and input.delta_y) {
        return case_l_a_x_y(*input.length, *input.angle, *input.delta_x, *input.delta_y);
    }
    // Case 2: {length, angle, delta_x}
    if(input.length and input.angle and input.delta_x) {
        return case_l_a_x(*input.length, *input.angle, *input.delta_x);
    }
    // Case 3: {length, angle, delta_y}
    if(input.length and input.angle and input.delta_y) {
        return case_l_a_y(*input.length, *input.angle, *input.delta_y);
    }
    // Case 4: {length, delta_x, delta_y}
    if(input.length and input.delta_x and input.delta_y) {
        return case_l_x_y(*input.length, *input.delta_x, *input.delta_y);
    }
    // Case 5: {angle, delta_x, delta_y}
    if(input.angle and input.delta_x and input.delta_y) {
        return case_a_x_y(*input.angle, *input.delta_x, *input.delta_y);
    }
    // Case 6: {length, angle}
    if(input.length and input.angle) {
        return case_l_a(*input.length, *input.angle);
    }
    // Case 7: {length, delta_x}
    if(input.length and input.delta_x) {
        return case_l_x(*input.length, *input.delta_x);
    }
    // Case 8: {length, delta_y}
    if(input.length and input.delta_y) {
        return case_l_y(*input.length, *input.delta_y);
    }
    // Case 9: {angle, delta_x}
    if(input.angle and input.delta_x) {
        return case_a_x(*input.angle, *input.delta_x);
    }
    // Case 10: {angle, delta_y}
    if(input.angle and input.delta_y) {
        return case_a_y(*input.angle, *input.delta_y);
    }
    // Case 11: {delta_x, delta_y}
    if(input.delta_x and input.delta_y) {
        return case_x_y(*input.delta_x, *input.delta_y);
    }

    throw std::runtime_error("Segment not fully specified, at least two constraints are needed.");
}

int Segment::n_coeffs() {
    return 8;
}

double Segment::length() const {
    auto f = [&](double t){ return hypot(dxdt(t), dydt(t)); };
    return AdaptiveSimpson::integrate<double>(f, 0.0, 1.0, 1e-8);    // Magic number
}

double Segment::angle() const {
    auto f = [&](double t){ return (dxdt(t)*dydt2(t) - dydt(t)*dxdt2(t))/(pow(dxdt(t), 2) + pow(dydt(t), 2)); };
    return AdaptiveSimpson::integrate<double>(f, 0.0, 1.0, 1e-8);    // Magic number
}

double Segment::energy() const {
    auto f = [&](double t){ return pow(dxdt(t)*dydt2(t) - dydt(t)*dxdt2(t), 2)/pow(pow(dxdt(t), 2) + pow(dydt(t), 2), 2.5); };
    return AdaptiveSimpson::integrate<double>(f, 0.0, 1.0, 1e-8);    // Magic number
}

double Segment::phi(double t) const{
    return atan2(dydt(t), dxdt(t));
}

double Segment::x(double t) const {
    return c[0] + t*(c[1] + t*(c[2] + t*c[3]));
}

double Segment::y(double t) const {
    return c[4] + t*(c[5] + t*(c[6] + t*c[7]));    // Todo: Use Horner evaluation
}

double Segment::dxdt(double t) const {
    return c[1] + t*(2.0*c[2] + 3.0*t*c[3]);    // Todo: Use Horner evaluation
}

double Segment::dydt(double t) const {
    return c[5] + t*(2.0*c[6] + 3.0*t*c[7]);    // Todo: Use Horner evaluation
}

double Segment::dxdt2(double t) const {
    return 2.0*c[2] + 6.0*c[3]*t;
}

double Segment::dydt2(double t) const {
    return 2.0*c[6] + 6.0*c[7]*t;
}

#include <iostream>

Vector<8> Segment::grad_length() const {
    auto f = [&](double t) -> Vector<8> {
        return (dxdt(t)*grad_dxdt(t) + dydt(t)*grad_dydt(t))/hypot(dxdt(t), dydt(t));
    };
    return AdaptiveSimpson::integrate<Vector<8>>(f, 0.0, 1.0, 1e-8);    // Magic number
}

Vector<8> Segment::grad_angle() const {
    return grad_phi(1) - grad_phi(0);
}

Vector<8> Segment::grad_energy() const {
    auto a = [&](double t) {
        return dxdt(t)*dydt2(t) - dydt(t)*dxdt2(t);
    };

    auto b = [&](double t) {
        return pow(dxdt(t), 2) + pow(dydt(t), 2);
    };

    auto grad_a = [&](double t) -> Vector<8> {
        return dydt2(t)*grad_dxdt(t) + dxdt(t)*grad_dydt2(t) - dxdt2(t)*grad_dydt(t) - dydt(t)*grad_dxdt2(t);
    };

    auto grad_b = [&](double t) -> Vector<8> {
        return 2.0*dxdt(t)*grad_dxdt(t) + 2.0*dydt(t)*grad_dydt(t);
    };

    auto grad_dedt = [&](double t) -> Vector<8> {
        return a(t)/pow(b(t), 5)*(2.0*pow(b(t), 2.5)*grad_a(t) - 2.5*a(t)*pow(b(t), 1.5)*grad_b(t));
    };

    return AdaptiveSimpson::integrate<Vector<8>>(grad_dedt, 0.0, 1.0, 1e-8);    // Magic number
}

Vector<8> Segment::grad_phi(double t) const {
    return (grad_dydt(t)*dxdt(t) - grad_dxdt(t)*dydt(t))/(pow(dxdt(t), 2) + pow(dydt(t), 2));
}

Vector<8> Segment::grad_x(double t) const {
    return { 1.0, t, t*t, t*t*t, 0.0, 0.0, 0.0, 0.0 };
}

Vector<8> Segment::grad_y(double t) const {
    return { 0.0, 0.0, 0.0, 0.0, 1.0, t, t*t, t*t*t };
}

Vector<8> Segment::grad_dxdt(double t) const {
    return { 0.0, 1.0, 2.0*t, 3.0*t*t, 0.0, 0.0, 0.0, 0.0 };
}

Vector<8> Segment::grad_dydt(double t) const {
    return { 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 2.0*t, 3.0*t*t };
}

Vector<8> Segment::grad_dxdt2(double t) const {
    return { 0.0, 0.0, 2.0, 6.0*t, 0.0, 0.0, 0.0, 0.0 };
}

Vector<8> Segment::grad_dydt2(double t) const {
    return { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2.0, 6.0*t };
}

std::vector<SegmentInput> ProfileCurve::input_from_matrix(const MatrixXd& matrix) {
    auto to_optional = [](double value) -> std::optional<double> {
        if(std::isnan(value))
            return std::nullopt;
        else
            return value;
    };

    std::vector<SegmentInput> input;
    for(int i = 0; i  < matrix.rows(); ++i) {
        input.push_back({
            .length  = to_optional(matrix(i, 0)),
            .angle   = to_optional(matrix(i, 1)),
            .delta_x = to_optional(matrix(i, 2)),
            .delta_y = to_optional(matrix(i, 3)),
        });
    }

    return input;
}

std::optional<std::string> ProfileCurve::validate_input(const std::vector<SegmentInput>& input) {
    // Count number of fully specified segments
    size_t n = std::count_if(input.begin(), input.end(), [](const SegmentInput& in) { return in.dimension() >= 2; });
    if(n == 0) {
        return std::string("At least one segment has to be specified");
    }

    for(size_t i = 0; i < input.size(); ++i) {
        std::optional<std::string> error = input[i].validate();
        if(error) {
            return "Segment " + std::to_string(i+1) + ": " + (*error);
        }
    }

    return std::nullopt;
}

ProfileCurve::ProfileCurve(const std::vector<SegmentInput>& input) {
    std::optional<std::string> error = validate_input(input);
    if(error.has_value()) {
        throw std::invalid_argument(error->c_str());
    }

    Point startpoint = { .x = 0.0, .y = 0.0, .phi = 0.0 };
    std::vector<Vector<8>> coeffs = optimize_local(startpoint, input);

    // Global optimization
    // ...

    // Assign segments
    for(auto& c: coeffs) {
        segments.push_back(Segment(c));
    }

    // Calculate arc length transform
    auto dldt = [&](double t) {
        size_t n = segments.size();
        size_t i = std::min((size_t) floor(n*t), n-1);
        return n*hypot(segments[i].dxdt(n*t - i), segments[i].dydt(n*t - i));
    };

    std::vector<double> t = linspace(0.0, 1.0, 10000);    // Magic number
    std::vector<double> s(t.size());
    for(size_t i = 1; i < t.size(); ++i) {
        s[i] = s[i-1] + (t[i] - t[i-1])/6.0*(dldt(t[i-1]) + 4.0*dldt((t[i] + t[i-1])/2.0) + dldt(t[i]));    // Integration by Simpson's rule
    }
    transform = LinearSpline(s, t);

    // Assign node points
    points.push_back({ .x = segments[0].x(0.0), .y = segments[0].y(0.0), .phi = segments[0].phi(0.0) });
    for(auto& segment: segments) {
        points.push_back({ .x = segment.x(1.0), .y = segment.y(1.0), .phi = segment.phi(1.0) });
    }
}

const std::vector<Segment>& ProfileCurve::get_segments() const {
    return segments;
}

const std::vector<Point>& ProfileCurve::get_nodes() const {
    return points;
}

double ProfileCurve::length() const {
    return transform.arg_max();
}

// Evaluate curve at a specific arc length. Returns {x, y, phi}.
Point ProfileCurve::operator()(double s) const {
    double t = transform(s);
    size_t n = segments.size();
    size_t i = std::min((size_t) floor(n*t), n-1);

    return {
        .x = segments[i].x(n*t - i),
        .y = segments[i].y(n*t - i),
        .phi = segments[i].phi(n*t - i)
    };
}

Vector<8> ProfileCurve::optimize_segment(const Point& point, const SegmentInput& input) {
    // Magic numbers
    const double ftol_rel = 1e-6;
    const double ftol_abs = 1e-9;
    const double ctol_abs = 1e-6;
    const int maxeval = 100;

    Vector<8> c = Segment::estimate_coeffs(point, input);
    std::vector<double> x_min(c.begin(), c.end());

    // Optimization algorithm and objective
    nlopt::opt opt(nlopt::algorithm::LD_SLSQP, x_min.size());
    opt.set_min_objective(objective, nullptr);
    opt.set_ftol_rel(ftol_rel);
    opt.set_ftol_abs(ftol_abs);
    opt.set_maxeval(maxeval);

    std::list<ConstraintData1> contexts;

    // Starting point constraints
    contexts.push_back(ConstraintData1{ .offset = 0, .value = point.phi });
    opt.add_equality_constraint(constraint_phi0, &contexts.back(), ctol_abs);

    contexts.push_back(ConstraintData1{ .offset = 0, .value = point.x });
    opt.add_equality_constraint(constraint_x0, &contexts.back(), ctol_abs);

    contexts.push_back(ConstraintData1{ .offset = 0, .value = point.y });
    opt.add_equality_constraint(constraint_y0, &contexts.back(), ctol_abs);

    // Input constraints
    if(input.length) {
        contexts.push_back(ConstraintData1{ .offset = 0, .value = *input.length });
        opt.add_equality_constraint(constraint_length, &contexts.back(), ctol_abs);
    }
    if(input.angle) {
        contexts.push_back(ConstraintData1{ .offset = 0, .value = *input.angle });
        opt.add_equality_constraint(constraint_angle, &contexts.back(), ctol_abs);
    }
    if(input.delta_x) {
        contexts.push_back(ConstraintData1{ .offset = 0, .value = *input.delta_x });
        opt.add_equality_constraint(constraint_delta_x, &contexts.back(), ctol_abs);
    }
    if(input.delta_y) {
        contexts.push_back(ConstraintData1{ .offset = 0, .value = *input.delta_y });
        opt.add_equality_constraint(constraint_delta_y, &contexts.back(), ctol_abs);
    }

    // Perform optimization
    try {
        opt.optimize(x_min);
    }
    catch(nlopt::roundoff_limited& e) {
        throw std::runtime_error("NLOPT_ROUNDOFF_LIMITED");
    }
    catch(nlopt::forced_stop& e) {
        throw std::runtime_error("NLOPT_FORCED_STOP");
    }
    catch(std::bad_alloc& e) {
        throw std::runtime_error("NLOPT_OUT_OF_MEMORY");
    }
    catch(std::invalid_argument& e) {
        throw std::runtime_error("NLOPT_INVALID_ARGS");
    }
    catch(std::runtime_error& e) {
        // Ignore this exception, it occurs often and for unknown reasons while the optimization result is still good
    }

    return Vector<8>(x_min.data());
}

std::vector<Vector<8>> ProfileCurve::optimize_local(const Point& startpoint, const std::vector<SegmentInput>& input) {
    std::vector<Vector<8>> coeffs;
    Point point = startpoint;
    for(auto& in: input) {
        coeffs.push_back(optimize_segment(point, in));
        Segment segment(coeffs.back());
        point = { .x = segment.x(1.0), .y = segment.y(1.0), .phi = segment.phi(1.0) };
    }

    return coeffs;
}

std::vector<Vector<8>> ProfileCurve::optimize_global(const std::vector<Vector<8>>& coeffs, const Point& startpoint, const std::vector<SegmentInput>& input) {
    // Magic numbers
    const double ftol_rel = 1e-6;
    const double ftol_abs = 1e-9;
    const double ctol_abs = 1e-6;
    const int maxeval = 100;

    // Concatenate all segment coefficients to form global coefficient vector
    std::vector<double> x_min;
    for(auto& c: coeffs) {
        x_min.insert(x_min.begin(), c.begin(), c.end());
    }

    // Optimization algorithm and objective
    nlopt::opt opt(nlopt::algorithm::LD_SLSQP, x_min.size());
    opt.set_min_objective(objective, nullptr);
    opt.set_ftol_rel(ftol_rel);
    opt.set_ftol_abs(ftol_abs);
    opt.set_maxeval(maxeval);

    std::list<ConstraintData1> contexts1;
    std::list<ConstraintData2> contexts2;

    // Starting point constraints
    contexts1.push_back(ConstraintData1{ .offset = 0, .value = startpoint.phi });
    opt.add_equality_constraint(constraint_phi0, &contexts1.back(), ctol_abs);

    contexts1.push_back(ConstraintData1{ .offset = 0, .value = startpoint.x });
    opt.add_equality_constraint(constraint_x0, &contexts1.back(), ctol_abs);

    contexts1.push_back(ConstraintData1{ .offset = 0, .value = startpoint.y });
    opt.add_equality_constraint(constraint_y0, &contexts1.back(), ctol_abs);

    // Input constraints
    for(size_t i = 0; i < input.size(); ++i) {
        size_t offset = i*Segment::n_coeffs();
        if(input[i].length) {
            contexts1.push_back(ConstraintData1{ .offset = offset, .value = *input[i].length });
            opt.add_equality_constraint(constraint_length, &contexts1.back(), ctol_abs);
        }
        if(input[i].angle) {
            contexts1.push_back(ConstraintData1{ .offset = offset, .value = *input[i].angle });
            opt.add_equality_constraint(constraint_angle, &contexts1.back(), ctol_abs);
        }
        if(input[i].delta_x) {
            contexts1.push_back(ConstraintData1{ .offset = offset, .value = *input[i].delta_x });
            opt.add_equality_constraint(constraint_delta_x, &contexts1.back(), ctol_abs);
        }
        if(input[i].delta_y) {
            contexts1.push_back(ConstraintData1{ .offset = offset, .value = *input[i].delta_y });
            opt.add_equality_constraint(constraint_delta_y, &contexts1.back(), ctol_abs);
        }
    }

    for(size_t i = 0; i < input.size() - 1; ++i) {
        size_t offset0 = (i + 0)*Segment::n_coeffs();
        size_t offset1 = (i + 1)*Segment::n_coeffs();

        contexts2.push_back(ConstraintData2{ .offset0 = offset0, .offset1 = offset1 });

        opt.add_equality_constraint(constraint_x, &contexts2.back(), ctol_abs);
        opt.add_equality_constraint(constraint_y, &contexts2.back(), ctol_abs);

        opt.add_equality_constraint(constraint_dxdt, &contexts2.back(), ctol_abs);
        opt.add_equality_constraint(constraint_dydt, &contexts2.back(), ctol_abs);

        opt.add_equality_constraint(constraint_dxdt2, &contexts2.back(), ctol_abs);
        opt.add_equality_constraint(constraint_dydt2, &contexts2.back(), ctol_abs);
    }

    // Perform optimization
    try {
        opt.optimize(x_min);
    }
    catch(nlopt::roundoff_limited& e) {
        throw std::runtime_error("NLOPT_ROUNDOFF_LIMITED");
    }
    catch(nlopt::forced_stop& e) {
        throw std::runtime_error("NLOPT_FORCED_STOP");
    }
    catch(std::bad_alloc& e) {
        throw std::runtime_error("NLOPT_OUT_OF_MEMORY");
    }
    catch(std::invalid_argument& e) {
        throw std::runtime_error("NLOPT_INVALID_ARGS");
    }
    catch(std::runtime_error& e) {
        // Ignore this exception, it occurs often and for unknown reasons while the optimization result is still good
    }

    std::vector<Vector<8>> result;
    for(size_t i = 0; i < input.size(); i += Segment::n_coeffs()) {
        result.push_back(Vector<8>(x_min.data() + i));
    }

    return result;
}

double ProfileCurve::objective(const std::vector<double>& x, std::vector<double>& grad, void* data) {
    double result = 0.0;
    for(size_t offset = 0; offset < x.size(); offset += Segment::n_coeffs()) {
        Vector<8> c(x.data() + offset);
        Segment segment(c);
        if(!grad.empty()) {
            std::fill(grad.begin(), grad.end(), 0.0);
            copy_to_vec(segment.grad_energy(), grad, offset);
        }

        result += segment.energy();
    }

    return result;
}

double ProfileCurve::constraint_phi0(const std::vector<double>& x, std::vector<double>& grad, void *data) {
    auto context = static_cast<ConstraintData1*>(data);
    Segment segment(Vector<8>(x.data() + context->offset));
    if(!grad.empty()) {
        std::fill(grad.begin(), grad.end(), 0.0);
        copy_to_vec(segment.grad_phi(0.0), grad, context->offset);
    }
    return segment.phi(0.0) - context->value;
};

double ProfileCurve::constraint_x0(const std::vector<double>& x, std::vector<double>& grad, void *data) {
    auto context = static_cast<ConstraintData1*>(data);
    Segment segment(Vector<8>(x.data() + context->offset));
    if(!grad.empty()) {
        std::fill(grad.begin(), grad.end(), 0.0);
        copy_to_vec(segment.grad_x(0.0), grad, context->offset);
    }
    return segment.x(0.0) - context->value;
}

double ProfileCurve::constraint_y0(const std::vector<double>& x, std::vector<double>& grad, void *data) {
    auto context = static_cast<ConstraintData1*>(data);
    Segment segment(Vector<8>(x.data() + context->offset));
    if(!grad.empty()) {
        std::fill(grad.begin(), grad.end(), 0.0);
        copy_to_vec(segment.grad_y(0.0), grad, context->offset);
    }
    return segment.y(0.0) - context->value;
}

double ProfileCurve::constraint_length(const std::vector<double>& x, std::vector<double>& grad, void *data) {
    auto context = static_cast<ConstraintData1*>(data);
    Segment segment(Vector<8>(x.data() + context->offset));
    if(!grad.empty()) {
        std::fill(grad.begin(), grad.end(), 0.0);
        copy_to_vec(segment.grad_length(), grad, context->offset);
    }
    return segment.length() - context->value;
}

double ProfileCurve::constraint_angle(const std::vector<double>& x, std::vector<double>& grad, void *data) {
    auto context = static_cast<ConstraintData1*>(data);
    Segment segment(Vector<8>(x.data() + context->offset));
    if(!grad.empty()) {
        std::fill(grad.begin(), grad.end(), 0.0);
        copy_to_vec(segment.grad_angle(), grad, context->offset);
    }
    return segment.angle() - context->value;
}

double ProfileCurve::constraint_delta_x(const std::vector<double>& x, std::vector<double>& grad, void *data) {
    auto context = static_cast<ConstraintData1*>(data);
    Segment segment(Vector<8>(x.data() + context->offset));
    if(!grad.empty()) {
        std::fill(grad.begin(), grad.end(), 0.0);
        copy_to_vec(segment.grad_x(1.0) - segment.grad_x(0.0), grad, context->offset);
    }
    return segment.x(1.0) - segment.x(0.0) - context->value;
}

double ProfileCurve::constraint_delta_y(const std::vector<double>& x, std::vector<double>& grad, void *data) {
    auto context = static_cast<ConstraintData1*>(data);
    Segment segment(Vector<8>(x.data() + context->offset));
    if(!grad.empty()) {
        std::fill(grad.begin(), grad.end(), 0.0);
        copy_to_vec(segment.grad_y(1.0) - segment.grad_y(0.0), grad, context->offset);
    }
    return segment.y(1.0) - segment.y(0.0) - context->value;
}

double ProfileCurve::constraint_x(const std::vector<double>& x, std::vector<double>& grad, void *data) {
    auto context = static_cast<ConstraintData2*>(data);
    Segment segment0(Vector<8>(x.data() + context->offset0));
    Segment segment1(Vector<8>(x.data() + context->offset1));

    if(!grad.empty()) {
        std::fill(grad.begin(), grad.end(), 0.0);
        copy_to_vec(-segment0.grad_x(0.0), grad, context->offset0);
        copy_to_vec(segment1.grad_x(1.0), grad, context->offset1);
    }

    return segment1.x(0) - segment0.x(1);
}

double ProfileCurve::constraint_y(const std::vector<double>& x, std::vector<double>& grad, void *data) {
    auto context = static_cast<ConstraintData2*>(data);
    Segment segment0(Vector<8>(x.data() + context->offset0));
    Segment segment1(Vector<8>(x.data() + context->offset1));

    if(!grad.empty()) {
        std::fill(grad.begin(), grad.end(), 0.0);
        copy_to_vec(-segment0.grad_y(0.0), grad, context->offset0);
        copy_to_vec(segment1.grad_y(1.0), grad, context->offset1);
    }

    return segment1.y(0) - segment0.y(1);
}

double ProfileCurve::constraint_dxdt(const std::vector<double>& x, std::vector<double>& grad, void *data) {
    auto context = static_cast<ConstraintData2*>(data);
    Segment segment0(Vector<8>(x.data() + context->offset0));
    Segment segment1(Vector<8>(x.data() + context->offset1));

    if(!grad.empty()) {
        std::fill(grad.begin(), grad.end(), 0.0);
        copy_to_vec(-segment0.grad_dxdt(0.0), grad, context->offset0);
        copy_to_vec(segment1.grad_dxdt(1.0), grad, context->offset1);
    }

    return segment1.dxdt(0) - segment0.dxdt(1);
}

double ProfileCurve::constraint_dydt(const std::vector<double>& x, std::vector<double>& grad, void *data) {
    auto context = static_cast<ConstraintData2*>(data);
    Segment segment0(Vector<8>(x.data() + context->offset0));
    Segment segment1(Vector<8>(x.data() + context->offset1));

    if(!grad.empty()) {
        std::fill(grad.begin(), grad.end(), 0.0);
        copy_to_vec(-segment0.grad_dydt(0.0), grad, context->offset0);
        copy_to_vec(segment1.grad_dydt(1.0), grad, context->offset1);
    }

    return segment1.dydt(0) - segment0.dydt(1);
}

double ProfileCurve::constraint_dxdt2(const std::vector<double>& x, std::vector<double>& grad, void *data) {
    auto context = static_cast<ConstraintData2*>(data);
    Segment segment0(Vector<8>(x.data() + context->offset0));
    Segment segment1(Vector<8>(x.data() + context->offset1));

    if(!grad.empty()) {
        std::fill(grad.begin(), grad.end(), 0.0);
        copy_to_vec(-segment0.grad_dxdt2(0.0), grad, context->offset0);
        copy_to_vec(segment1.grad_dxdt2(1.0), grad, context->offset1);
    }

    return segment1.dxdt(0) - segment0.dxdt(1);
}

double ProfileCurve::constraint_dydt2(const std::vector<double>& x, std::vector<double>& grad, void *data) {
    auto context = static_cast<ConstraintData2*>(data);
    Segment segment0(Vector<8>(x.data() + context->offset0));
    Segment segment1(Vector<8>(x.data() + context->offset1));

    if(!grad.empty()) {
        std::fill(grad.begin(), grad.end(), 0.0);
        copy_to_vec(-segment0.grad_dydt2(0.0), grad, context->offset0);
        copy_to_vec(segment1.grad_dydt2(1.0), grad, context->offset1);
    }

    return segment1.dydt2(0) - segment0.dydt2(1);
}

void ProfileCurve::copy_to_vec(const Vector<8>& source, std::vector<double>& target, size_t offset) {
    std::copy(source.begin(), source.end(), target.begin() + offset);
}
