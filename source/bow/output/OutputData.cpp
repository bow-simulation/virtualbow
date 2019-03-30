#include "OutputData.hpp"
#include <fstream>

void OutputData::save(const std::string& path) const
{
    std::vector<uint8_t> buffer = json::to_msgpack(*this);
    std::ofstream file(path);
    file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
}
