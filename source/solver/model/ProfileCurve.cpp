#include "ProfileCurve.hpp"
#include "solver/numerics/Integration.hpp"
#include "solver/numerics/Linspace.hpp"
#include "solver/numerics/Eigen.hpp"
#include <nlopt.hpp>

Segment::Segment(const std::vector<double>& c): c(c) {
    if(c.size() != 8) {
        throw std::invalid_argument("Coefficient vector must be of size 8");
    }
}

std::vector<double> Segment::estimate_coeffs(const Point& point, const SegmentInput& input) {
    // Normalize angle to a range of [-pi, pi]
    // https://stackoverflow.com/a/24234924
    auto normalize_angle = [](double x) {
        return x - 2*M_PI*floor((x + M_PI)/(2*M_PI));
    };

    // Calculate coefficients for a line starting at the initial point with angle phi and length l
    auto linear = [&](double phi, double l) -> std::vector<double> {
        return { point.x, l*cos(phi), 0, 0, point.y, l*sin(phi), 0, 0 };
    };

    // Given: {dl, dphi}
    if(input.length && input.angle) {
        auto from_points = [](Vector<2> p0, Vector<2> p1, Vector<2> p2, Vector<2> p3) -> Vector<8> {
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

        double l = *input.length/3.0;
        double a = *input.angle/4.0;

        Vector<2> p0{ point.x, point.y };
        Vector<2> p1 = p0 + Vector<2>{ l*cos(point.phi + 1.0*a), l*sin(point.phi + 1.0*a) };
        Vector<2> p2 = p1 + Vector<2>{ l*cos(point.phi + 2.0*a), l*sin(point.phi + 2.0*a) };
        Vector<2> p3 = p2 + Vector<2>{ l*cos(point.phi + 3.0*a), l*sin(point.phi + 3.0*a) };

        Vector<8> c = from_points(p0, p1, p2, p3);
        return std::vector<double>(c.begin(), c.end());
    }

    // Given: {dl, dx}
    if(input.length && input.delta_x) {
        double dl_n = std::max(*input.length, abs(*input.delta_x));    // If dl < dx, use dx as length
        double phi_n = normalize_angle(point.phi);
        double alpha = acos(*input.delta_x/dl_n);

        if(phi_n >= 0.0) {
            if(*input.delta_x >= 0.0) {
                return linear(alpha, *input.length);
            } else {
                return linear(M_PI - alpha, *input.length);
            }
        } else {
            if(*input.delta_x >= 0.0) {
                return linear(-alpha, *input.length);
            } else {
                return linear(alpha - M_PI, *input.length);
            }
        }
    }

    // Given: {dl, dy}
    if(input.length && input.delta_y) {
        double dl_n = std::max(*input.length, abs(*input.delta_y));    // If dl < dy, use dy as length
        double phi_n = normalize_angle(point.phi);
        double alpha = acos(*input.delta_y/dl_n);

        if(phi_n >= -M_PI_2 && phi_n < M_PI_2) {
            if(*input.delta_y >= 0.0) {
                return linear(M_PI_2 - alpha, *input.length);
            } else {
                return linear(-M_PI_2 + alpha, *input.length);
            }
        } else {
            if(*input.delta_y >= 0.0) {
                return linear(M_PI_2 + alpha, *input.length);
            } else {
                return linear(-M_PI_2 - alpha, *input.length);
            }
        }
    }

    // Given: {dphi, dx}
    if(input.angle && input.delta_x) {
        if(*input.delta_x >= 0.0) {
            return linear(0, *input.delta_x);
        } else {
            return linear(M_PI, -*input.delta_x);
        }
    }

    // Given: {dphi, dy}
    if(input.angle && input.delta_y) {
        if(*input.delta_y >= 0.0) {
            return linear(M_PI_2, *input.delta_y);
        } else {
            return linear(-M_PI_2, -*input.delta_y);
        }
    }

    // Given: {dx, dy}
    if(input.delta_x && input.delta_y) {
        return linear(atan2(*input.delta_y, *input.delta_x), hypot(*input.delta_x, *input.delta_y));
    }

    // Fallback
    return linear(point.phi, 1);
}

const std::vector<double>& Segment::get_coeffs() const {
    return c;
}

void Segment::set_coeffs(const std::vector<double>&  c) {
    this->c = c;
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

std::vector<SegmentInput> ProfileCurve::to_input(const MatrixXd& matrix) {
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

std::optional<std::string> ProfileCurve::validate(const std::vector<SegmentInput>& input) {
    if(input.empty()) {
        return std::string("At least one segment is required");
    }

    for(auto& in: input) {
        int n = in.angle.has_value() + in.length.has_value() + in.delta_x.has_value() + in.delta_y.has_value();
        if(n < 2) {
            return "At least two values per segment are needed";
        }
    }
}

ProfileCurve::ProfileCurve(const std::vector<SegmentInput>& input) {
    /*
    std::optional<std::string> error = validate(input);
    if(error.has_value()) {
        throw std::invalid_argument(error->c_str());
    }
    */

    // Local optimization of the segments
    Point point = { .x = 0.0, .y = 0.0, .phi = 0.0 };
    for(auto& in: input) {
        if(in.is_valid()) {
            Segment segment = optimize_local(point, in);
            segments.push_back(segment);
            point = { .x = segment.x(1.0), .y = segment.y(1.0), .phi = segment.phi(1.0) };
        }
    }

    // Global optimization
    // ...

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

Segment ProfileCurve::optimize_local(const Point& point, const SegmentInput& input) {
    struct ContextData {
        Point point;
        SegmentInput input;
    };

    auto objective_function = [](const std::vector<double>& c, std::vector<double>& grad, void* data) {
        Segment segment(c);
        if(!grad.empty()) {
            Vector<8> g = segment.grad_energy();
            std::copy(g.begin(), g.end(), grad.begin());
        }
        return segment.energy();
    };

    auto constraint_x0 = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        Segment segment(c);
        if(!grad.empty()) {
            Vector<8> g = segment.grad_x(0.0);
            std::copy(g.begin(), g.end(), grad.begin());
        }
        return segment.x(0.0) - context->point.x;
    };

    auto constraint_y0 = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        Segment segment(c);
        if(!grad.empty()) {
            Vector<8> g = segment.grad_y(0.0);
            std::copy(g.begin(), g.end(), grad.begin());
        }
        return segment.y(0.0) - context->point.y;
    };

    auto constraint_phi0 = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        Segment segment(c);
        if(!grad.empty()) {
            Vector<8> g = segment.grad_phi(0.0);
            std::copy(g.begin(), g.end(), grad.begin());
        }
        return segment.phi(0.0) - context->point.phi;
    };

    auto constraint_length = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        Segment segment(c);
        if(!grad.empty()) {
            Vector<8> g = segment.grad_length();
            std::copy(g.begin(), g.end(), grad.begin());
        }
        return segment.length() - *(context->input.length);
    };

    auto constraint_angle = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        Segment segment(c);
        if(!grad.empty()) {
            Vector<8> g = segment.grad_angle();
            std::copy(g.begin(), g.end(), grad.begin());
        }
        return segment.angle() - *(context->input.angle);
    };

    auto constraint_delta_x = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        Segment segment(c);
        if(!grad.empty()) {
            Vector<8> g = segment.grad_x(1.0) - segment.grad_x(0.0);
            std::copy(g.begin(), g.end(), grad.begin());
        }
        return segment.x(1.0) - segment.x(0.0) - *(context->input.delta_x);
    };

    auto constraint_delta_y = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        Segment segment(c);
        if(!grad.empty()) {
            Vector<8> g = segment.grad_y(1.0) - segment.grad_y(0.0);
            std::copy(g.begin(), g.end(), grad.begin());
        }
        return segment.y(1.0) - segment.y(0.0) - *(context->input.delta_y);
    };

    const double ftol_rel = 1e-6;     // Magic number
    const double ftol_abs = 1e-9;     // Magic number
    const double ctol_abs = 1e-6;     // Magic number
    const int maxeval = 500;          // Magic number

    double f_min;
    std::vector<double> c_min = Segment::estimate_coeffs(point, input);
    ContextData context = { .point = point, .input = input };

    // Todo: Remove
    // return Segment(c_min);

    nlopt::opt opt(nlopt::algorithm::LD_SLSQP, c_min.size());
    opt.set_min_objective(objective_function, &context);
    opt.set_ftol_rel(ftol_rel);
    opt.set_ftol_abs(ftol_abs);
    opt.set_maxeval(maxeval);

    opt.add_equality_constraint(constraint_x0, &context, ctol_abs);
    opt.add_equality_constraint(constraint_y0, &context, ctol_abs);
    opt.add_equality_constraint(constraint_phi0, &context, ctol_abs);

    if(input.length) {
        opt.add_equality_constraint(constraint_length, &context, ctol_abs);
    }
    if(input.angle) {
        opt.add_equality_constraint(constraint_angle, &context, ctol_abs);
    }
    if(input.delta_x) {
        opt.add_equality_constraint(constraint_delta_x, &context, ctol_abs);
    }
    if(input.delta_y) {
        opt.add_equality_constraint(constraint_delta_y, &context, ctol_abs);
    }

    try {
        opt.optimize(c_min, f_min);
    }
    catch(std::exception& e) {
        throw std::runtime_error("Optimization failed");
    }

    return Segment(c_min);
}
