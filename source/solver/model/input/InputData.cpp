#include "InputData.hpp"
#include "Conversion.hpp"
#include <fstream>
#include <iomanip>

InputData::InputData(const std::string& path)
{
    std::ifstream stream(path);
    json obj; obj << stream;

    Conversion::to_current(obj);
    from_json(obj, *this);
}

void InputData::save(const std::string& path) const
{
    json obj;
    to_json(obj, *this);

    std::ofstream stream(path);
    stream << std::setw(4) << obj << std::endl;
}
