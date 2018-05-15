/*
#include <iostream>
#include "numerics/CubicSpline.hpp"

int main()
{
    std::vector<double> x = {0.0, 1.0, 2.0, 3.0};
    std::vector<double> y = {1.0, 1.1, -1.0, -1.1};

    Series data = CubicSpline(Series(x, y)).sample(6);

    for(auto& val: data.vals())
        std::cout << val << "\n";

    return 0;
}
*/

#include "gui/Application.hpp"

int main(int argc, char* argv[])
{
    return Application::run(argc, argv);
}
