#include "InputData.hpp"
#include "gui/Application.hpp"

#include <fstream>

void InputData::load(const std::string& path)
{
    std::ifstream stream(path);

    json obj;
    obj << stream;

    load(obj);
}

void InputData::save(const std::string& path)
{
    json obj;
    save(obj);

    std::ofstream stream(path);
    stream << std::setw(4) << obj << std::endl;
}
