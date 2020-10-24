/*
#include "solver/numerics/Linspace.hpp"
#include "solver/numerics/Integration.hpp"
#include "solver/numerics/LinearSpline.hpp"
#include "solver/numerics/Eigen.hpp"
#include <nlopt.hpp>
#include <optional>
#include <vector>
#include <cmath>
#include <algorithm>

struct Point {
    double x;
    double y;
    double phi;
};

struct SegmentInput {
    std::optional<double> delta_x;
    std::optional<double> delta_y;
    std::optional<double> length;
    std::optional<double> angle;
};

class SegmentCurve {
public:
    // Segment curve from polynomial coefficients
    SegmentCurve(const std::vector<double>& c): c(c) {
        if(c.size() != 8) {
            throw std::invalid_argument("Coefficient vector must be of size 8");
        }
    }

    static std::vector<double> estimate_coeffs(const Point& point, const SegmentInput& input) {
        return {point.x, cos(point.phi), 0.0, 0.0, point.y, sin(point.phi), 0.0, 0.0};
    }

    const std::vector<double>& get_coeffs() const {
        return c;
    }

    void set_coeffs(const std::vector<double>&  c) {
        this->c = c;
    }

    double length() const {
        auto f = [&](double t){ return hypot(dxdt(t), dydt(t)); };
        return AdaptiveSimpson::integrate<double>(f, 0, 1, 1e-8);    // Magic number
    }

    double angle() const {
        auto f = [&](double t){ return (dxdt(t)*dydt2(t) - dydt(t)*dxdt2(t))/(pow(dxdt(t), 2) + pow(dydt(t), 2)); };
        return AdaptiveSimpson::integrate<double>(f, 0, 1, 1e-8);    // Magic number
    }

    double energy() const {
        auto f = [&](double t){ return pow(dxdt(t)*dydt2(t) - dydt(t)*dxdt2(t), 2)/pow(pow(dxdt(t), 2) + pow(dydt(t), 2), 2.5); };
        return AdaptiveSimpson::integrate<double>(f, 0, 1, 1e-8);    // Magic number
    }

    // Orientation angle in [-PI, PI]
    double phi(double t) const{
        return atan2(dydt(t), dxdt(t));
    }

    double x(double t) const {
        return c[0] + c[1]*t + c[2]*t*t + c[3]*t*t*t;    // Todo: Use Horner evaluation
    }

    double y(double t) const {
        return c[4] + c[5]*t + c[6]*t*t + c[7]*t*t*t;    // Todo: Use Horner evaluation
    }

    double dxdt(double t) const {
        return c[1] + 2.0*c[2]*t + 3.0*c[3]*t*t;    // Todo: Use Horner evaluation
    }

    double dydt(double t) const {
        return c[5] + 2.0*c[6]*t + 3.0*c[7]*t*t;    // Todo: Use Horner evaluation
    }

    double dxdt2(double t) const {
        return 2.0*c[2] + 6.0*c[3]*t;
    }

    double dydt2(double t) const {
        return 2.0*c[6] + 6.0*c[7]*t;
    }

private:
    std::vector<double> c;
};


SegmentCurve optimize_local(const Point& point, const SegmentInput& input) {
    struct ContextData {
        Point point;
        SegmentInput input;
    };

    auto objective_function = [](const std::vector<double>& c, std::vector<double>& grad, void* data) {
        if(!grad.empty()) {
            throw std::invalid_argument("Function does not provide gradient information");
        }
        return SegmentCurve(c).energy();
    };

    auto constraint_x0 = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        SegmentCurve segment(c);
        return segment.x(0.0) - context->point.x;
    };

    auto constraint_y0 = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        SegmentCurve segment(c);
        return segment.y(0.0) - context->point.y;
    };

    auto constraint_phi0 = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        SegmentCurve segment(c);
        return segment.phi(0.0) - context->point.phi;
    };

    auto constraint_delta_x = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        SegmentCurve segment(c);
        return segment.x(1.0) - segment.x(0.0) - *(context->input.delta_x);
    };

    auto constraint_delta_y = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        SegmentCurve segment(c);
        return segment.y(1.0) - segment.y(0.0) - *(context->input.delta_y);
    };

    auto constraint_length = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        SegmentCurve segment(c);
        return segment.length() - *(context->input.length);
    };

    auto constraint_angle = [](const std::vector<double>& c, std::vector<double>& grad, void *data) {
        auto context = static_cast<ContextData*>(data);
        SegmentCurve segment(c);
        return segment.angle() - *(context->input.angle);
    };

    const double epsilon_f = 1e-6;    // Magic number
    const double epsilon_c = 1e-8;    // Magic number
    const int maxeval = 100;          // Magic number

    double f_min;
    std::vector<double> c_min = SegmentCurve::estimate_coeffs(point, input);
    ContextData context = { .point = point, .input = input };

    nlopt::opt opt(nlopt::algorithm::LN_COBYLA, c_min.size());
    opt.set_min_objective(objective_function, &context);
    opt.set_xtol_rel(epsilon_f);
    opt.set_maxeval(maxeval);

    opt.add_equality_constraint(constraint_x0, &context, epsilon_c);
    opt.add_equality_constraint(constraint_y0, &context, epsilon_c);
    opt.add_equality_constraint(constraint_phi0, &context, epsilon_c);

    if(input.delta_x) {
        opt.add_equality_constraint(constraint_delta_x, &context, epsilon_c);
    }
    if(input.delta_y) {
        opt.add_equality_constraint(constraint_delta_y, &context, epsilon_c);
    }
    if(input.length) {
        opt.add_equality_constraint(constraint_length, &context, epsilon_c);
    }
    if(input.angle) {
        opt.add_equality_constraint(constraint_angle, &context, epsilon_c);
    }

    if(opt.optimize(c_min, f_min) < 0) {
        throw std::runtime_error("Failed to determine profile curve.");
    }

    return SegmentCurve(c_min);
}

class ProfileCurve {
public:
    ProfileCurve(const std::vector<SegmentInput>& input) {
        // Local optimization of the segments
        Point point = { .x = 0.0, .y = 0.0, .phi = 0.0 };
        for(auto& in: input) {
            SegmentCurve segment = optimize_local(point, in);
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

        // Assign points
        points.push_back({ .x = segments[0].x(0.0), .y = segments[0].y(0.0), .phi = segments[0].phi(0.0) });
        for(auto& segment: segments) {
            points.push_back({ .x = segment.x(1.0), .y = segment.y(1.0), .phi = segment.phi(1.0) });
        }
    }

    const std::vector<Point>& get_points() const {
        return points;
    }

    double length() const {
        return transform.arg_max();
    }

    // Evaluate curve at a specific arc length. Returns {x, y, phi}.
    Vector<3> operator()(double s) const {
        double t = transform(s);
        size_t n = segments.size();
        size_t i = std::min((size_t) floor(n*t), n-1);

        return {
            segments[i].x(n*t - i),
            segments[i].y(n*t - i),
            segments[i].phi(n*t - i)
        };
    }

    double x(double s) const {
        double t = transform(s);
        size_t n = segments.size();
        size_t i = std::min((size_t) floor(n*t), n-1);
        return segments[i].x(n*t - i);

    }

    double y(double s) const {
        double t = transform(s);
        size_t n = segments.size();
        size_t i = std::min((size_t) floor(n*t), n-1);
        return segments[i].y(n*t - i);
    }

private:
    std::vector<SegmentCurve> segments;
    std::vector<Point> points;
    LinearSpline transform;
};

#include <iostream>

int main() {

    std::vector<SegmentInput> input {
        { .delta_x = 1.0, .delta_y = 0.5, .length = std::nullopt, .angle = std::nullopt },
        { .delta_x = 1.0, .delta_y = -0.5, .length = std::nullopt, .angle = std::nullopt }
    };

    ProfileCurve curve(input);
    for(double t: Linspace(0.0, curve.length(), 50)) {
        Vector<3> pt = curve(t);
        std::cout << pt[0] << "\t" << pt[1] << "\n";
    }

    std::cout << curve.length();

}
*/

/*
int main() {

    Point point{ .x = 0.0, .y = 0.0, .phi = 0.0 };
    SegmentInput input{ .delta_x = 1.0, .delta_y = 1.0, .length = std::nullopt, .angle = std::nullopt };

    SegmentCurve segment = optimize_local(point, input);

    for(double t: Linspace(0.0, 1.0, 50)) {
        std::cout << segment.x(t) << "\t" << segment.y(t) << "\n";
    }

    std::cout << segment.length();
}
*/

#include "MainWindow.hpp"
#include "config.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName(Config::ORGANIZATION_NAME);
    app.setOrganizationDomain(Config::ORGANIZATION_DOMAIN);
    app.setApplicationName(Config::APPLICATION_NAME_GUI);
    app.setApplicationDisplayName(Config::APPLICATION_DISPLAY_NAME_GUI);
    app.setApplicationVersion(Config::APPLICATION_VERSION);
    app.setAttribute(Qt::AA_DontShowIconsInMenus, true);

    QLocale::setDefault(QLocale::C);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "Model file (.bow)");
    parser.process(app);

    QStringList args = parser.positionalArguments();
    if(args.size() > 1) {
        std::cerr << "Only one argument is accepted." << std::endl;
        return 1;
    }

    MainWindow window;
    window.show();
    if(args.size() == 1) {
        window.loadFile(args[0]);
    }

    return app.exec();
}
