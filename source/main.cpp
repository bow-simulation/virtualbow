#include "numerics/Math.hpp"
#include "numerics/RootFinding.hpp"
#include <iostream>

// Returns the real roots of the quadratic polynomial
// f(x) = c0 + c1*x + c2*x^2
// or NaN if no real solutions exist.
Vector<2> solve_quadratic(double c0, double c1, double c2)
{
    double s = sqrt(c1*c1 - 4.0*c2*c0);
    return {(-c1 + s)/(2.0*c2), (-c1 - s)/(2.0*c2)};
}

struct Points
{
    VectorXd x;
    VectorXd y;
};

// Partitions the curve of linear segments given by x_in, y_in into n_out points such that
// the first and last point coincide with the start and end of the curve and all points
// are evenly spaced out by euclidean distance.
// Assumption: Euclidean distance between points is monotonously rising with the curve's arc length
// (Only affects calculation of the error measure)
Points equipartition(VectorXd x_in, VectorXd y_in, size_t n_out)
{
    assert(x_in.size() == y_in.size());
    assert(x_in.size() >= 2);
    assert(n_out >= 2);

    size_t n_in = x_in.size();      // Number of input points
    VectorXd x_out(n_out);          // Output data
    VectorXd y_out(n_out);

    // Calculate lenths of the input curve
    VectorXd s_in(n_in);
    s_in[0] = 0.0;
    for(size_t i = 1; i < n_in; ++i)
        s_in[i] = s_in[i-1] + hypot(x_in[i] - x_in[i-1], y_in[i] - y_in[i-1]);

    // Assign first and last points
    x_out[0] = x_in[0];
    y_out[0] = y_in[0];

    x_out[n_out-1] = x_in[n_in-1];
    y_out[n_out-1] = y_in[n_in-1];

    // Function that returns two values eta_1/2 such that
    // ||A + eta_i*(B-A), C|| = d.
    auto find_by_distance = [](Vector<2> A, Vector<2> B, Vector<2> C, double d)
    {
        double x_ab = B[0] - A[0];  double x_ac = C[0] - A[0];
        double y_ab = B[1] - A[1];  double y_ac = C[1] - A[1];

        return solve_quadratic(x_ac*x_ac + y_ac*y_ac - d*d,
                              -2.0*(x_ac*x_ab + y_ac*y_ab),
                               x_ab*x_ab+ y_ab*y_ab);
    };

    // Function that calculates the intermediate points for a fixed distance
    // d and returns a measure of error. The real equipartition is done by finding
    // the root of this function.
    auto partition = [&](double d)
    {
        // i: Input point index
        // j: Output point index.
        // Iterate over all intermediate output points...
        for(size_t i = 0, j = 1; j < n_out; ++j)
        {
            while(true)
            {
                // Try to find a point on the current input segment (i, i+1)
                // with distance d to the previous output point (j-1)
                double eta = find_by_distance({x_in[i], y_in[i]}, {x_in[i+1], y_in[i+1]},
                                              {x_out[j-1], y_out[j-1]}, d).maxCoeff();

                if(eta > 1.0 && i < n_in-2)    // If eta lies outside the current segment and it's not the last segment: move to the next segment.
                {
                    ++i;
                }
                else if(j < n_out-1)    // Intermediate point: Calculate position and assign
                {
                    x_out[j] = x_in[i] + eta*(x_in[i+1] - x_in[i]);
                    y_out[j] = y_in[i] + eta*(y_in[i+1] - y_in[i]);
                    break;
                }
                else    // End point: Don't assign, calculate arc lenth to end of input curve
                {
                    double l = (1.0 - eta)*(s_in[i+1] - s_in[i]);
                    for(i = i+1; i < n_in-1; ++i)
                        l += s_in[i+1] - s_in[i];

                    return l/s_in[n_in-1];    // Return arc length relative to total length as error
                }
            }
        }
    };

    // Find root of the partition function, use input curve lenth divided by number of
    // output segments as a reasonable initial value for the distance d.
    double d = s_in[n_in-1]/(n_out-1);
    secant_method(partition, d, 1.1*d, 1e-6, 50);    // Magic numbers
    //bracket_and_bisect<false>(partition, d, 2.0, 1e-6, 1e-6, 50);    // Magic numbers

    return {x_out, y_out};
}



int main()
{
    VectorXd x(10), y(10);
    x << 0.0, 0.0, 1.0, 3.0, 4.0, 4.0, 3.0, 2.0, 2.0, 3.0;
    y << 0.0, 1.0, 3.0, 4.0, 3.0, 2.0, 1.0, 1.0, 2.0, 2.0;

    Points out = equipartition(x, y, 12);
    std::cout << "x = " << out.x.transpose() << "\n";
    std::cout << "y = " << out.y.transpose() << "\n";

    for(size_t i = 0; i < out.x.size()-1; ++i)
    {
        std::cout << hypot(out.x[i+1] - out.x[i], out.y[i+1] - out.y[i]) << "\n";
    }

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
