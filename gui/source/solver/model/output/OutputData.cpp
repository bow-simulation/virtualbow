#include "OutputData.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

using nlohmann::json;

OutputData::OutputData(const std::string& path)
{
    // https://github.com/nlohmann/json/issues/479
    std::ifstream stream(path, std::ios::binary);
    stream.exceptions(~std::ofstream::goodbit);    // Make stream throw exception on failure
    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    json obj = json::from_msgpack(buffer);
    from_json(obj, *this);
}

void OutputData::save(const std::string& path) const
{
    // https://github.com/nlohmann/json/issues/479
    std::vector<uint8_t> buffer = json::to_msgpack(*this);
    std::ofstream stream(path, std::ios::out | std::ios::binary);
    stream.exceptions(~std::ofstream::goodbit);    // Make stream throw exception on failure
    stream.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
}
