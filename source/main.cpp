/*
#include "numerics/Eigen.hpp"
#include "bow/input/InputData.hpp"
#include "numerics/ArcCurve.hpp"

struct LimbProperties2
{
    // Geometry
    VectorXd s;
    VectorXd x;
    VectorXd y;
    VectorXd phi;
    VectorXd w;
    VectorXd h;

    // Section properties
    VectorXd rhoA;   // Linear density
    VectorXd Cee;    // Longitudinal stiffness
    VectorXd Ckk;    // Bending stiffness
    VectorXd Cek;    // Coupling term

    // n: Number of points
    LimbProperties2(const InputData& input, unsigned n)
        : s(n), x(n), y(n), phi(n), w(n), h(n), rhoA(n), Cee(n), Ckk(n), Cek(n)
    {
        // 1. Profile curve
        Curve2D curve = ArcCurve::sample(input.profile.segments, input.profile.x0,
                                         input.profile.y0, input.profile.phi0, n-1);

        // Todo: Is there a more elegant way? Maybe have a Curve2D member?
        s = curve.s;
        x = curve.x;
        y = curve.y;
        phi = curve.phi;

        // 2. Section properties
        Series width = CubicSpline::sample(input.width, n_elements_limb);
    }
};


int main()
{
    InputData input;
    LimbProperties2 limb(input, 10);



    return 0;
}
*/

#include "gui/Application.hpp"

int main(int argc, char* argv[])
{
    return Application::run(argc, argv);
}

/*
#include "gui/EditableTabBar.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    // QApplication::setStyle("windows");

    EditableTabBar tb;
    tb.addTab("Layer0");
    tb.addTab("Layer1");
    tb.addTab("Layer");
    tb.show();

    QObject::connect(&tb, &EditableTabBar::addTabRequested, []()
    {
        qInfo() << "Add Tab!";
    });

    QObject::connect(&tb, &EditableTabBar::tabRenamed, [](int index, const QString& name)
    {
        qInfo() << index << " renamed to " << name;
    });

    return app.exec();
}
*/
