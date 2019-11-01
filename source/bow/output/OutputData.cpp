#include "OutputData.hpp"
#include <fstream>

void OutputData::save(const std::string& path) const
{
	// https://github.com/nlohmann/json/issues/479#issuecomment-282988808
    std::vector<uint8_t> buffer = json::to_msgpack(*this);
    std::ofstream file(path, std::ios::out | std::ios::binary);
    file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
}
