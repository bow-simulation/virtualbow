/*
#include <iostream>

int main()
{
    std::vector<double> x = {0.0, 0.0, 2.0, 3.0};
    std::vector<double> y = {0.0, 1.0, 4.0, 9.0};

    CubicSpline spline = CubicSpline(x, y);

    std::vector<double> xi = {0.5, 1.5, 2.5};
    std::vector<double> yi = spline.interpolate(xi);

    for(auto& val: yi)
        std::cout << val << "\n";

    return 0;
}
*/

#include "gui/Application.hpp"

int main(int argc, char* argv[])
{
    return Application::run(argc, argv);
}
