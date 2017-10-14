/*
#include <iostream>

#include <string>
#include <vector>

template<typename T>
class Invalidatable
{
public:
    Invalidatable()
        : valid(false)
    {

    }

    void invalidate()
    {
        valid = false;
    }

    bool is_valid() const
    {
        return valid;
    }

    operator T&()
    {
        if(!valid)
            throw std::runtime_error("Trying to access an invalid value");

        return value;
    }

    operator const T&() const
    {
        if(!valid)
            throw std::runtime_error("Trying to access an invalid value");

        return value;
    }

    template<typename S>
    Invalidatable& operator=(const S& rhs)
    {
        value = rhs;
        valid = true;
        return *this;
    }

private:
    T value;
    bool valid;
};


int main()
{
    Invalidatable<double> x;

    x = 2.0;

    std::cout << "valid: " << x.is_valid() << "\n";
    std::cout << "value: " << x + 5.0 << "\n";

    return 0;
}
*/

/*
struct Meta
{
    std::string version;
    std::string comments;
};

struct Profile
{
    std::vector<double> length;
    std::vector<double> curvature;
    double x0;
    double y0;
    double phi0;
};

struct Spline
{
    std::vector<double> positions;
    std::vector<double> values;
};

struct Material
{
    double rho;
    double E;
};

struct String
{
    double strand_stiffness;
    double strand_density;
    double n_strands;
};

struct Masses
{
    double strin_center;
    double string_tip;
    double limb_tip;
};

struct Operation
{
    double brace_height;
    double draw_length;
    double arrow_mass;
};

struct Settings
{
    unsigned n_elements_limb;
    unsigned n_elements_string;
    unsigned n_draw_steps;
    double time_span_factor;
    double time_step_factor;
    double sampling_rate;
};

struct InputData
{
    Meta meta;
    Profile profile;
    Spline width;
    Spline height;
    Material material;
    String string;
    Masses masses;
    Operation operation;
    Settings settings;
};
*/

#include "gui/Application.hpp"

int main(int argc, char* argv[])
{
    return Application::run(argc, argv);
}


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

    DynamicSolver solver(system, 0.1, 1e-3, [&]{ return system.get_t() >= 0.5; });
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
