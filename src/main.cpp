#include "numerics/StepFunction.hpp"
#include "numerics/SplineFunction.hpp"

#include <iostream>

int main()
{
    std::vector<double> l = {4.0, 5.0, 2.0, 5.0, 3.0};
    std::vector<double> v = {1.0, 2.0, 3.0, 1.0, -1.0};

    StepFunction fn(l, v);

    std::cout << fn(25);

    return 0;
}

/*
#include "gui/MainWindow.hpp"
#include <QtWidgets>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
*/
