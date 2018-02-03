/*
#include "numerics/Eigen.hpp"
#include "bow/input/InputData.hpp"
#include "bow/LimbProperties.hpp"
#include "numerics/ArcCurve.hpp"

int main()
{
    InputData input;
    LimbProperties limb(input, 10);

    return 0;
}
*/

/*
#include <vector>
#include <numeric>
#include <iostream>

std::vector<double> get_section_centers(const std::vector<double>& heights)
{
    double h_total = std::accumulate(heights.begin(), heights.end(), 0.0);

    std::vector<double> z_center;

    double z_bottom = -0.5*h_total;
    for(double h: heights)
    {
        z_center.push_back(z_bottom + 0.5*h);
        z_bottom += h;
    }

    return z_center;
}


int main()
{
    std::vector<double> heights{1.0, 2.0, 3.0, 4.0, 5.0};

    std::vector<double> z_center = get_section_centers(heights);

    for(double z: z_center)
        std::cout << z << "\n";

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
