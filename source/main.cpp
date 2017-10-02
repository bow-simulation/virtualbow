#include "fem/System.hpp"
#include "fem/Solver.hpp"
#include "fem/elements/ContactSurface.hpp"
#include "fem/elements/BarElement.hpp"

#include "numerics/Math.hpp"
#include <iostream>
#include <vector>

int main()
{
    unsigned n = 10;    // Number of nodes on the contact surface
    double r = 1.0;
    double h = 0.2;

    double ks = 1e1;
    double kc = 1e5;

    System system;
    std::vector<Node> master_nodes;
    std::vector<Node> slave_nodes;

    for(unsigned i = 0; i < n; ++i)
    {
        double phi = double(i)/(n-1)*M_PI_2;
        master_nodes.push_back(system.create_node({DofType::Fixed , DofType::Fixed , DofType::Fixed},
        { 1.0 - r*sin(phi), 1.0 - r*cos(phi), M_PI-phi}));
    }

    Node origin = system.create_node({DofType::Fixed , DofType::Fixed , DofType::Fixed}, {0.0, 0.0, 0.0});
    Node point = system.create_node({DofType::Active, DofType::Active, DofType::Fixed}, {0.95*r, 0.95*h, 0.0});

    slave_nodes.push_back(point);
    system.add_element(ContactSurface(master_nodes, slave_nodes, VectorXd::Constant(n, h), kc));

    double l = (M_SQRT2 - 1.0)/2.0;
    system.add_element(BarElement(origin, point, l, ks*l, 10.0), "bar");

    /*
    DynamicSolver solver(system, 0.2, 0.01, 500.0);
    while(solver.step())
        std::cout << "d = " << hypot(point1[0].u(), point1[1].u()) << "\n";
    */

    //system.q();

    StaticSolverLC solver(system);
    solver.find_equilibrium();

    std::cout << "x = " << point[0].u() << ", y = " << point[1].u() << "\n";
    //std::cout << "E = " << system.get_potential_energy("bar") << "\n";
}

/*
#include "gui/Application.hpp"

int main(int argc, char* argv[])
{
    return Application::run(argc, argv);
}
*/

/*
#include "numerics/Math.hpp"
#include "numerics/Geometry.hpp"
#include <iostream>

int main()
{
    std::vector<Vector<2>> input = {
        { 0.0, -2.0},
        { 0.0,  0.0},
        { 2.0,  0.0},
        { 6.0, -1.0},
        { 9.0, -2.0},
        {11.0, -2.0},
        {12.0, -1.0},
        {14.0,  0.0},
        {16.0, -1.0},
        {18.0,  0.0},
        {19.0,  2.0},
    };

    auto output = constant_orientation_subset(input, true);

    for(auto& point: output)
        std::cout << point.transpose() << "\n";
}
*/

/*
#include "fem/System.hpp"
#include "fem/Solver.hpp"
#include "fem/elements/ContactElement.hpp"
#include "fem/elements/MassElement.hpp"

#include "numerics/Math.hpp"
#include <iostream>

int main()
{
    System system;
    Node node_a = system.create_node({DofType::Fixed , DofType::Fixed , DofType::Fixed}, { 0.0, 0.0, M_PI_2});
    Node node_b = system.create_node({DofType::Fixed , DofType::Fixed , DofType::Fixed}, { 0.0, 1.0, M_PI_2});
    Node node_c = system.create_node({DofType::Active, DofType::Active, DofType::Fixed}, { 1.0, 0.5, 0.0});

    ContactElement contact(node_a, node_b, node_c, 0.5, 0.5, 5000.0);
    system.add_element(contact);

    system.add_element(MassElement(node_c, 1.0, 0.0));
    node_c[0].p_mut() = -10.0;

    DynamicSolver solver(system, 0.02, 0.01, 500.0);
    while(solver.step())
    {
        std::cout << "x = " << node_c[0].u() << ", y = " << node_c[1].u() << "\n";

    }
}
*/

/*
Vector2d p0{0.0, 0.0};
Vector2d p1{2.0, 1.0};
Vector2d p2{0.0, 1.0};

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
            "lA[1]ers",
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
        button->setMA[0]imumSize(20, 20);
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

    virtual void tabLA[1]outChange()
    {
        // This virtual handler is called whenever the tab lA[1]out changes.
        // If anything changes make sure the plus button is in the correct location.
        QTabBar::tabLA[1]outChange();
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
