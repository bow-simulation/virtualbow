#include "model/ProfileCurve2.hpp"
#include "numerics/Linspace.hpp"
#include <iostream>

int main()
{
        std::vector<Point> points = {
            {.s = 0.0, .phi = 0.0, .x = 0.0, .y = 0.0},
            {.s = 1.0, .phi = 0.5, .x = NAN, .y = NAN},
            {.s = 1.5, .phi = NAN, .x = 1.3, .y = NAN},
            {.s = NAN, .phi = NAN, .x = 1.5, .y = 0.5}
        };

        ProfileCurve2 curve(points);
        for(double s: Linspace<double>(curve.s_min(), curve.s_max(), 50))
        {
            std::cout << curve(s)(0) << "\t" << curve(s)(1) << "\n";
        }

    return 0;
}

/*
#include <math.h>
#include <nlopt.hpp>
#include <iostream>
#include <functional>

typedef struct
{
    double a;
    double b;
} my_constraint_data;

int main()
{
    auto objective = [](const std::vector<double>& x, std::vector<double>& grad, void* data) {
        if(!grad.empty()) {
            grad[0] = 0.0;
            grad[1] = 0.5/sqrt(x[1]);
        }

        return sqrt(x[1]);
    };

    auto constraints = [](const std::vector<double>& x, std::vector<double>& grad, void *data) {
        my_constraint_data *d = (my_constraint_data *) data;
        double a = d->a, b = d->b;

        if(!grad.empty()) {
            grad[0] = 3 * a * (a*x[0] + b) * (a*x[0] + b);
            grad[1] = -1.0;
        }

        return ((a*x[0] + b) * (a*x[0] + b) * (a*x[0] + b) - x[1]);
    };

    my_constraint_data data[2] = { {2,0}, {-1,1} };


    nlopt::opt opt(nlopt::algorithm::LN_COBYLA, 2);
    opt.set_min_objective(objective, nullptr);
    opt.add_equality_constraint(constraints, &data[0], 1e-8);
    opt.add_equality_constraint(constraints, &data[1], 1e-8);
    opt.set_xtol_rel(1e-4);

    std::vector<double> xmin = { 1.234, 5.678 };
    double fmin;

    if (opt.optimize(xmin, fmin) < 0)
    {
        printf("nlopt failed!\n");
    }
    else
    {
        printf("found minimum at f(%g,%g) = %0.10g\n", xmin[0], xmin[1], fmin);
    }
}
*/

/*
#include <QtCore>
#include "config.hpp"
#include "model/BowModel.hpp"
#include <utility>
#include <iostream>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    app.setOrganizationName(Config::ORGANIZATION_NAME);
    app.setOrganizationDomain(Config::ORGANIZATION_DOMAIN);
    app.setApplicationName(Config::APPLICATION_NAME_SLV);
    app.setApplicationVersion(Config::APPLICATION_VERSION);

    QLocale::setDefault(QLocale::C);

    QCommandLineOption statics({"s", "static"}, "Run a static simulation.");
    QCommandLineOption dynamics({"d", "dynamic"}, "Run a dynamic simulation.");
    QCommandLineOption progress({"p", "progress"}, "Print simulation progress.");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(statics);
    parser.addOption(dynamics);
    parser.addOption(progress);
    parser.addPositionalArgument("input", "Model file (.bow)");
    parser.addPositionalArgument("output", "Result file (.res)");
    parser.process(app);

    QStringList args = parser.positionalArguments();

    if(args.size() == 0)
    {
        parser.showHelp();
        return 0;
    }
    else if(args.size() > 2)
    {
        std::cerr << "Too many arguments." << std::endl;;
        return 1;
    }

    QString input_path = args[0];
    QString output_path;

    if(args.size() == 2)
    {
        output_path = args[1];
    }
    else
    {
        QFileInfo info(input_path);
        output_path = info.absolutePath() + QDir::separator() + info.completeBaseName() + ".res";
    }

    try
    {
        SimulationMode mode;
        if(parser.isSet(dynamics))
        {
            mode = SimulationMode::Dynamic;
        }
        else if(parser.isSet(statics))
        {
            mode = SimulationMode::Static;
        }
        else
        {
            std::cerr << "Simulation mode not set." << std::endl;;
            return 1;
        }

        InputData input(input_path.toStdString());

        std::pair<int, int> previous = {-1, -1};
        OutputData output = BowModel::simulate(input, mode, [&](int p1, int p2) {
            if(p1 != previous.first || p2 != previous.second)
            {
                previous = {p1, p2};
                if(parser.isSet(progress))
                {
                    std::cout << p1 << "\t" << p2 << std::endl;
                }
            }
        });

        output.save(output_path.toStdString());
        return 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
*/
