#include "numerics/Math.hpp"
#include <iostream>

// Returns the real roots of the quadratic polynomial
// f(x) = c0 + c1*x + c2*x^2
// or NaN if no real solutions exist.
Vector<2> solve_quadratic(double c0, double c1, double c2)
{
    double s = sqrt(c1*c1 - 4.0*c2*c0);
    return {(-c1 + s)/(2.0*c2), (-c1 - s)/(2.0*c2)};
}

Vector<2> find_by_distance(Vector<2> A, Vector<2> B, Vector<2> C, double d)
{
    double x_ab = B[0] - A[0];  double x_ac = C[0] - A[0];
    double y_ab = B[1] - A[1];  double y_ac = C[1] - A[1];

    return solve_quadratic(x_ac*x_ac + y_ac*y_ac - d*d,
                          -2.0*(x_ac*x_ab + y_ac*y_ab),
                           x_ab*x_ab+ y_ab*y_ab);
}

struct Points
{
    VectorXd x;
    VectorXd y;
};



double equalize(Vector<2> A, Vector<2> B, Vector<2> C, Vector<2> D)
{
    return 0.5*(pow(A[0]-D[0], 2) + pow(A[1]-D[1], 2) - pow(A[0]-C[0], 2) - pow(A[1] - C[1], 2))
              /((B[0]-A[0])*(D[0]-C[0]) + (B[1]-A[1])*(D[1]-C[1]));
}

Points equipartition(VectorXd x, VectorXd y, size_t k)
{
    assert(x.size() == y.size());
    assert(x.size() > 1);

    size_t n = x.size();    // Number of input points

    // Calculate lenths of all input sections
    VectorXd s(n);
    s[0] = 0.0;
    for(size_t i = 1; i < n; ++i)
    {
        s[i] = s[i-1] + hypot(x[i] - x[i-1], y[i] - y[i-1]);
    }

    VectorXd xp(k);
    VectorXd yp(k);
    VectorXi ip(k);

    // i: Index of input curve points
    // j: Index of output points
    for(size_t i = 0, j = 0; j < k; ++j)
    {
        double sj = double(j)/(k-1)*s[n-1];

        // Change input index until s[i] <= sj <= s[i+1]
        while(s[i+1] < sj && i < n-2)
        {
            ++i;
        }

        double eta = (sj - s[i])/(s[i+1] - s[i]);
        xp[j] = x[i] + eta*(x[i+1] - x[i]);
        yp[j] = y[i] + eta*(y[i+1] - y[i]);
        ip[j] = i;
    }

    for(size_t iter = 0; iter < 50; ++iter)
    {
        //std::cout << "xp0 = " << xp.transpose() << "\n";
        //std::cout << "yp0 = " << yp.transpose() << "\n\n";

        double delta_max = 0.0;

        // Iterate
        for(size_t j = 1; j < k-1; ++j)
        {
            size_t i = ip[j];
            double eta = equalize({x[i], y[i]}, {x[i+1], y[i+1]}, {xp[j-1], yp[j-1]}, {xp[j+1], yp[j+1]});

            while(eta < 0.0)
            {
                --i;
                eta = equalize({x[i], y[i]}, {x[i+1], y[i+1]}, {xp[j-1], yp[j-1]}, {xp[j+1], yp[j+1]});
            }

            while(eta > 1.0)
            {
                ++i;
                eta = equalize({x[i], y[i]}, {x[i+1], y[i+1]}, {xp[j-1], yp[j-1]}, {xp[j+1], yp[j+1]});
            }

            double xp_new = x[i] + eta*(x[i+1] - x[i]);
            double yp_new = y[i] + eta*(y[i+1] - y[i]);
            delta_max = std::max(delta_max, hypot(xp[j] - xp_new, yp[j] - yp_new)/s[n-1]);

            xp[j] = xp_new;
            yp[j] = yp_new;
            ip[j] = i;
        }

        std::cout << "delta_max: " << delta_max << "\n";
    }

    return {xp, yp};
}



int main()
{
    /*
    Vector<2> A{0, 0};
    Vector<2> B{1, 1};
    Vector<2> C{1, 0};
    Vector<2> D{2, 0};

    std::cout << equalize(A, B, C, D);
    */


    /*
    Vector<2> A{-1, 0};
    Vector<2> B{1, 0};
    Vector<2> C{1, 1};
    double d = 1.1;

    std::cout << find_B[1]_distance(A, B, C, d);
    */

    VectorXd x(6), y(6);
    x << 0.0, 1.0, 2.0, 3.0, 4.0, 5.0;
    y << 0.0, 1.0, 4.0, 9.0, 16.0, 25.0;

    Points out = equipartition(x, y, 50);

    //std::cout << "x = " << out.x.transpose() << "\n";
    //std::cout << "y = " << out.y.transpose() << "\n";

    //std::cout << "x = " << output.x.transpose() << "\n";
    //std::cout << "y = " << output.y.transpose() << "\n";
}


/*
#include "gui/Application.hpp"

int main(int argc, char* argv[])
{
    return Application::run(argc, argv);
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
