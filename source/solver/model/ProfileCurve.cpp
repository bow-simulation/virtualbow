#include "ProfileCurve.hpp"


Segment::Segment(const std::vector<double>& c): c(c) {
    if(c.size() != 8) {
        throw std::invalid_argument("Coefficient vector must be of size 8");
    }
}

std::vector<double> Segment::estimate_coeffs(const Point& point, const SegmentInput& input) {
    return { point.x, cos(point.phi), 0.0, 0.0, point.y, sin(point.phi), 0.0, 0.0 };
}

const std::vector<double>& Segment::get_coeffs() const {
    return c;
}

void Segment::set_coeffs(const std::vector<double>&  c) {
    this->c = c;
}

double Segment::length() const {
    auto f = [&](double t){ return hypot(dxdt(t), dydt(t)); };
    return AdaptiveSimpson::integrate<double>(f, 0, 1, 1e-8);    // Magic number
}

double Segment::angle() const {
    auto f = [&](double t){ return (dxdt(t)*dydt2(t) - dydt(t)*dxdt2(t))/(pow(dxdt(t), 2) + pow(dydt(t), 2)); };
    return AdaptiveSimpson::integrate<double>(f, 0, 1, 1e-8);    // Magic number
}

double Segment::energy() const {
    auto f = [&](double t){ return pow(dxdt(t)*dydt2(t) - dydt(t)*dxdt2(t), 2)/pow(pow(dxdt(t), 2) + pow(dydt(t), 2), 2.5); };
    return AdaptiveSimpson::integrate<double>(f, 0, 1, 1e-8);    // Magic number
}

double Segment::phi(double t) const{
    return atan2(dydt(t), dxdt(t));
}

double Segment::x(double t) const {
    return c[0] + c[1]*t + c[2]*t*t + c[3]*t*t*t;    // Todo: Use Horner evaluation
}

double Segment::y(double t) const {
    return c[4] + c[5]*t + c[6]*t*t + c[7]*t*t*t;    // Todo: Use Horner evaluation
}

double Segment::dxdt(double t) const {
    return c[1] + 2.0*c[2]*t + 3.0*c[3]*t*t;    // Todo: Use Horner evaluation
}

double Segment::dydt(double t) const {
    return c[5] + 2.0*c[6]*t + 3.0*c[7]*t*t;    // Todo: Use Horner evaluation
}

double Segment::dxdt2(double t) const {
    return 2.0*c[2] + 6.0*c[3]*t;
}

double Segment::dydt2(double t) const {
    return 2.0*c[6] + 6.0*c[7]*t;
}

ProfileCurve ProfileCurve::from_matrix(const MatrixXd& matrix) {
    auto to_optional = [](double value) {
        return !std::isnan(value) ? std::optional<double>(value) : std::nullopt;
    };

    std::vector<SegmentInput> input;
    for(int i = 0; i  < matrix.rows(); ++i) {
        if(!std::isnan(matrix(i, 0) && !std::isnan(matrix(i, 1)))) {
            input.push_back({
                .length  = to_optional(matrix(i, 0)),
                .angle   = to_optional(matrix(i, 1)),
                .delta_x = to_optional(matrix(i, 2)),
                .delta_y = to_optional(matrix(i, 3)),
            });
        }
    }

    return ProfileCurve(input);
}


ProfileCurve::ProfileCurve(const std::vector<SegmentInput>& input) {
    // Local optimization of the segments
    Point point = { .x = 0.0, .y = 0.0, .phi = 0.0 };
    for(auto& in: input) {
        Segment segment = optimize_local(point, in);
        segments.push_back(segment);
        point = { .x = segment.x(1.0), .y = segment.y(1.0), .phi = segment.phi(1.0) };
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
        if(!grad.empty()) {
            throw std::invalid_argument("Function does not provide gradient information");
        }
        return Segment(c).energy();
    };

    auto constraint_x0 = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        Segment segment(c);
        return segment.x(0.0) - context->point.x;
    };

    auto constraint_y0 = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        Segment segment(c);
        return segment.y(0.0) - context->point.y;
    };

    auto constraint_phi0 = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        Segment segment(c);
        return segment.phi(0.0) - context->point.phi;
    };

    auto constraint_length = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        Segment segment(c);
        return segment.length() - *(context->input.length);
    };

    auto constraint_angle = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        Segment segment(c);
        return segment.angle() - *(context->input.angle);
    };

    auto constraint_delta_x = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        Segment segment(c);
        return segment.x(1.0) - segment.x(0.0) - *(context->input.delta_x);
    };

    auto constraint_delta_y = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        Segment segment(c);
        return segment.y(1.0) - segment.y(0.0) - *(context->input.delta_y);
    };

    const double epsilon_f = 1e-6;    // Magic number
    const double epsilon_c = 1e-8;    // Magic number
    const int maxeval = 100;          // Magic number

    double f_min;
    std::vector<double> c_min = Segment::estimate_coeffs(point, input);
    ContextData context = { .point = point, .input = input };

    nlopt::opt opt(nlopt::algorithm::LN_COBYLA, c_min.size());
    opt.set_min_objective(objective_function, &context);
    opt.set_xtol_rel(epsilon_f);
    opt.set_maxeval(maxeval);

    opt.add_equality_constraint(constraint_x0, &context, epsilon_c);
    opt.add_equality_constraint(constraint_y0, &context, epsilon_c);
    opt.add_equality_constraint(constraint_phi0, &context, epsilon_c);

    if(input.length) {
        opt.add_equality_constraint(constraint_length, &context, epsilon_c);
    }
    if(input.angle) {
        opt.add_equality_constraint(constraint_angle, &context, epsilon_c);
    }
    if(input.delta_x) {
        opt.add_equality_constraint(constraint_delta_x, &context, epsilon_c);
    }
    if(input.delta_y) {
        opt.add_equality_constraint(constraint_delta_y, &context, epsilon_c);
    }

    if(opt.optimize(c_min, f_min) < 0) {
        throw std::runtime_error("Failed to determine profile curve.");
    }

    return Segment(c_min);
}
