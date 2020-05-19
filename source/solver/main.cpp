#include <QtCore>
#include "config.hpp"
#include "model/BowModel.hpp"
#include <utility>
#include <iostream>

#include <chrono>

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

        auto t1 = std::chrono::high_resolution_clock::now();

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

        auto t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
        std::cout << "Simulation time: " << duration << " ms\n";

        output.save(output_path.toStdString());
        return 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
