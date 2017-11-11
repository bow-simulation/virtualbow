#include "Application.hpp"
#include "MainWindow.hpp"
#include "model/BowModel.hpp"

#ifdef WIN32
#include <windows.h>
#endif

const std::string Application::version = "0.4";
Settings Application::settings = Settings();

int Application::run(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("bow-simulator");
    app.setApplicationDisplayName("Bow Simulator");
    app.setApplicationVersion(QString::fromStdString(version));
    app.setOrganizationDomain("bow-simulator.org");
    setlocale(LC_NUMERIC, "C");

    // Load settings
    settings.load();

    // Parse command line arguments
    QCommandLineOption statics({"s", "static"}, "Perform a static simulation.");
    QCommandLineOption dynamics({"d", "dynamic"}, "Perform a dynamic simulation.");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(statics);
    parser.addOption(dynamics);
    parser.addPositionalArgument("input", "Input file.");
    parser.addPositionalArgument("output", "Output file.");
    parser.process(app);

    auto pos_args = parser.positionalArguments();
    if(pos_args.size() > 2)
    {
        qInfo() << "Too many arguments.";
        return 1;
    }

    bool input_set = (pos_args.size() > 0);
    bool output_set = (pos_args.size() > 1);
    bool mode_set = (parser.isSet(statics) || parser.isSet(dynamics));

    int success;    // Return value

    if(output_set || mode_set)  // Run Batch
    {
        if(!input_set) {
            qInfo() << "No input file provided.";
            return 1;
        }

        QString input_path = pos_args[0];
        QString output_path;
        bool dynamics_set;

        if(output_set) {
            output_path = pos_args[1];
        }
        else {
            QFileInfo info(input_path);
            output_path = info.absolutePath() + QDir::separator() + info.completeBaseName() + ".dat";
        }

        if(mode_set) {
            dynamics_set = parser.isSet(dynamics);
        }
        else {
            dynamics_set = true;
        }

        success = run_cli(input_path, output_path, dynamics_set);
    }
    else // Run GUI
    {
        success = run_gui(app, input_set ? pos_args[0] : ":/bows/default.bow");
    }

    // Save settings
    settings.save();

    return success;
}

int Application::run_cli(QString input_path, QString output_path, bool dynamic)
{
    try
    {
        InputData input;
        input.load(input_path);
        OutputData output = dynamic ? BowModel::run_dynamic_simulation(input, [](int){return true;}, [](int){return true;})
                                    : BowModel::run_static_simulation(input, [](int){return true;});
        output.save(output_path.toStdString());
        return 0;
    }
    catch(const std::runtime_error& e)
    {
        qInfo() << "Error: " << e.what();
        return 1;
    }
}

int Application::run_gui(QApplication& app, QString path)
{
    try
    {
        #ifdef WIN32
        FreeConsole();
        #endif

        MainWindow window(path);
        window.show();
        return app.exec();
    }
    catch(const std::runtime_error& e)
    {
        qInfo() << "Error: " << e.what();
        return 1;
    }
}
