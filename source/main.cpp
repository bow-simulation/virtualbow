#include "gui/Application.hpp"

int main(int argc, char* argv[])
{
    return Application::run(argc, argv);
}

/*
#include <iostream>
#include "bow/ContinuousLimb.hpp"

int main()
{
    InputData input;
    ContinuousLimb limb(input);

    std::cout << limb.get_C(0.1) << "\n";

    return 0;
}
*/
