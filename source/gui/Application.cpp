#include "Application.hpp"
#include "MainWindow.hpp"
#include "bow/BowModel.hpp"
#include "config.hpp"

#ifdef WIN32
#include <windows.h>
#endif

QSettings Application::settings{Config::WEBSITE, Config::NAME};

int Application::run(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(Config::NAME);
    app.setApplicationDisplayName(Config::TITLE);
    app.setApplicationVersion(Config::VERSION);
    app.setOrganizationDomain(Config::WEBSITE);

    // Set default locale to C (English, US)
    QLocale::setDefault(QLocale::C);

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

        return runCLI(input_path, output_path, dynamics_set);
    }
    else
    {
        return runGUI(app, input_set ? pos_args[0] : "");
    }
}

int Application::runGUI(QApplication& app, QString path)
{
    try
    {
        #ifdef WIN32
        FreeConsole();
        #endif

        MainWindow window;
        window.show();

        if(!path.isEmpty())
            window.loadFile(path);

        return app.exec();
    }
    catch(const std::runtime_error& e)
    {
        qInfo() << "Error: " << e.what();
        return 1;
    }
}

int Application::runCLI(QString input_path, QString output_path, bool dynamic)
{
    try
    {
        InputData input;
        input.load(input_path.toStdString());

        /*
        if(input.get_errors().size() != 0)
        {
            qInfo() << "Error: " << QString::fromStdString(input.get_errors().front());
            return 1;
        }
        */

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
