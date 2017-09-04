#include "gui/MainWindow.hpp"
#include "model/BowModel.hpp"
#include "Version.hpp"

#include <fstream>

#ifdef WIN32
#include <windows.h>
#endif

int run_cli(QString input_path, QString output_path, bool dynamic)
{
    try
    {
        InputData input(input_path);
        OutputData output = dynamic ? BowModel::run_dynamic_simulation(input, [](int){return true;}, [](int){return true;})
                                    : BowModel::run_static_simulation(input, [](int){return true;});
        output.save(output_path.toStdString());
    }
    catch(const std::runtime_error& e)
    {
        qInfo() << "Error: " << e.what();
        return 1;
    }
}

int run_gui(QApplication& app, QString path)
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

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationDisplayName("Bow Simulator");
    app.setApplicationVersion(QString::fromStdString(version));
    app.setOrganizationDomain("https://bow-simulator.gitlab.io/");
    setlocale(LC_NUMERIC, "C");

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

        return run_cli(input_path, output_path, dynamics_set);
    }
    else // Run GUI
    {
        return run_gui(app, input_set ? pos_args[0] : ":/bows/default.bow");
    }

    /*
    if(!mode_set)
    {
        if(output_set) {
            qInfo() << "No simulation mode selected.";
            return 1;
        }

        return run_gui(app, input_set ? pos_args[0] : ":/bows/default.bow");
    }
    else
    {
        if(!input_set)
        {
            qInfo() << "No input file provided.";
            return 1;
        }

        QString input_path = pos_args[0];
        QString output_path;

        if(output_set)
        {
            output_path = pos_args[1];
        }
        else
        {
            QFileInfo info(input_path);
            output_path = info.absolutePath() + QDir::separator() + info.completeBaseName() + ".dat";
        }

        bool perform_dynamics = mode_set ? parser.isSet(dynamics) : true;
        return run_cli(input_path, output_path, perform_dynamics);
    }
    else
    {
        return run_gui(app, input_set ? pos_args[0] : ":/bows/default.bow");
    }
    */
}

/*
#include "gui/GuiApplication.hpp"

int main(int argc, char *argv[])
{
    return GuiApplication::start(argc, argv);
}
*/

/*
#include "utils/Json.hpp"
#include <iostream>

struct Point
{
    double x = 0.0;
    double y = 0.0;
};

void to_json(json& j, const Point& p)
{
    j = json{{"x", p.x}, {"y", p.y}};

    j["x"] = p.x;
    j["y"] = p.y;
}

void from_json(const json& j, Point& p)
{
    p.x = j.at("x");
    p.y = j.at("y");
}

int main()
{
    json j;
    j["test"] = Point();

    std::cout << j;

    return 0;
}
*/

/*
#include <json.hpp>
#include <iostream>
#include <fstream>

using nlohmann::json;

int main()
{

    json obj;
    obj["test"]["b"] = {0.0, 1.0, 2.0, 3.0, 4.0};

    std::cout << obj["xyz"].is_null();

    //std::vector<uint8_t> buffer = json::to_msgpack(obj);
    //std::ofstream os("file.dat");
    //os.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());

    //os << json::to_msgpack(obj) << std::endl;

    return 0;
}
*/

/*
#include "fem/System.hpp"
#include "fem/Solver.hpp"
#include "fem/elements/ContactElement.hpp"
#include "fem/elements/MassElement.hpp"

#include <Eigen/Dense>
#include <iostream>

int main()
{
    System system;
    Node node_a = system.create_node({DofType::Fixed , DofType::Fixed , DofType::Fixed}, {-1.0, 0.0, 0.0});
    Node node_b = system.create_node({DofType::Fixed , DofType::Fixed , DofType::Fixed}, { 1.0, 0.0, 0.0});
    Node node_c = system.create_node({DofType::Active, DofType::Active, DofType::Fixed}, { 0.0, 1.0, 0.0});

    system.add_element(ContactElement(node_a, node_b, node_c, 1.0, 0.0, 5000.0));

    system.add_element(MassElement(node_c, 1.0, 0.0));
    node_c[1].p_mut() = -10.0;

    DynamicSolver solver(system, 0.1, 1e-3, [&]{ return system.t() >= 0.5; });
    while(solver.step())
    {
        std::cout << node_c[0].u() << "," << node_c[1].u() << "\n";
    }

}
*/


/*
Eigen::Vector2d p0{0.0, 0.0};
Eigen::Vector2d p1{2.0, 1.0};
Eigen::Vector2d p2{0.0, 1.0};

auto orientation = [](auto p0, auto p2, auto p1)
{
    return (p1(0) - p0(0))*(p2(1) - p0(1)) - (p1(1) - p0(1))*(p2(0) - p0(0));
};

std::cout << orientation(p0, p1, p2);

return 0;
*/

/*
#include <iostream>
#include "model/document/DocumentNode.hpp"

int main()
{
    //std::map<std::string, DocumentNode> test = {{"A", Double(3.14)},{"B", Double(3.14)}};

    DocumentNode doc(
    {
        {
            "meta",
            {
                {  "version", Value<std::string>(All<std::string>()) },    // Todo*: Version
                { "comments", Value<std::string>(All<std::string>()) }
            }
        },

        {
            "profile",
            {
                { "arc length", Vector(Positive<std::vector<double>>()) },
                {  "curvature", Vector(All<std::vector<double>>()) },
                {   "offset x", Double(All<double>()) },
                {   "offset y", Double(All<double>()) },
                {      "angle", Double(All<double>()) }
            }
        },

        {
            "width",
            {
                Vector(InclusiveRange<std::vector<double>>(0.0, 1.0) && Ascending<std::vector<double>>()),
                Vector(Positive<std::vector<double>>())
            }
        },

        {
            "layers",
            {
                {
                    { "name", String(All<std::string>()) },
                    {  "rho", Double(Positive<double>()) },
                    {    "E", Double(Positive<double>()) },
                    {
                        "height",
                        {
                            Vector(InclusiveRange<std::vector<double>>(0.0, 100.0) && Ascending<std::vector<double>>()),
                            Vector(Positive<std::vector<double>>())
                        }
                    }
                }
            }
        },

        {
            "string",
            {
                { "strand stiffness", Double(Positive<double>()) },
                {   "strand density", Double(Positive<double>()) },
                {          "strands", Double(Positive<double>()) }
            }
        },

        {
            "masses",
            {
                { "string center", Double(!Negative<double>()) },
                {    "string tip", Double(!Negative<double>()) },
                {      "limb tip", Double(!Negative<double>()) }
            }
        },

        {
            "operation",
            {
                { "brace height", Double(All<double>()) },
                {  "draw length", Double(All<double>()) },
                {   "arrow mass", Double(Positive<double>()) }
            }
        },

        {
            "settings",
            {
                {    "elements limb", Integer(Positive<int>()) },
                {  "elements string", Integer(Positive<int>()) },
                {       "draw steps", Integer(Positive<int>()) },
                { "time span factor", Double(Positive<double>()) },
                { "time step factor", Double(ExclusiveRange<double>(0.0, 1.0)) },
                {    "sampling time", Double(Positive<double>()) }
            }
        }
    });

    return 0;
}
*/
/*
int main()
{
    std::vector<double> a = {0.0, 0.5, 1.0};
    std::cout << (a >= 0.0);

    return 0;
}
*/

/*
    DocItem(value, unit, name);
    double get_base() const;    // Replace with conversion operator to T.
    double get_scaled() const;
    void set_scaled(double value)

    T base_value;
    std::string name;
    Unit unit;
*/

/*
#include <QtWidgets>

class TabBarPlus: public QTabBar
{
public:
    TabBarPlus()
    {
        button = new QPushButton("+", this);
        button->setMaximumSize(20, 20);
        button->setMinimumSize(20, 20);

        setTabsClosable(true);
        movePlusButton();

        QObject::connect(this, &QTabBar::tabBarDoubleClicked, [&](int index)
        {
            qInfo() << "Double click";
        });

        QObject::connect(button, &QPushButton::clicked, [&]
        {
            qInfo() << "Add tab";
        });
    }

    virtual QSize sizeHint() const override
    {
        // Return the size of the TabBar with increased width for the plus button."""
        QSize sizeHint = QTabBar::sizeHint();
        sizeHint += {25, 0};

        return sizeHint;
    }

    virtual void resizeEvent(QResizeEvent * event) override
    {
        // Resize the widget and make sure the plus button is in the correct location.
        QTabBar::resizeEvent(event);
        movePlusButton();
    }

    virtual void tabLayoutChange()
    {
        // This virtual handler is called whenever the tab layout changes.
        // If anything changes make sure the plus button is in the correct location.
        QTabBar::tabLayoutChange();
        movePlusButton();
    }

    void movePlusButton()
    {
        // Move the plus button to the correct location.
        // Find the width of all of the tabs
        int size = 0;
        for(int i = 0; i < count(); ++i)
            size += tabRect(i).width();

        // Set the plus button location in a visible area
        int h = 0;  // geometry().top();

        button->move(size, h);
    }

private:
    QPushButton* button;
};


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TabBarPlus tb;
    tb.addTab("A");
    tb.addTab("B");
    tb.show();

    return app.exec();
}
*/
