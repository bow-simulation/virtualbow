#include "API.hpp"
#include <nlohmann/json.hpp>

using namespace nlohmann;
using ffi::Response;

// Checks the response an error.
// If an error message is present, convert it into an exception and deallocate the response.
void check_response(Response response) {
    if(response.error) {
        std::string message(response.error);
        ffi::free_response(response);
        throw SolverException(message);
    }
}

// Checks the response for an error.
// If no error is present, tries to interpret the avaiable msgpack data as an object of type T.
template<typename T>
T parse_response(Response response) {
    check_response(response);

    json object = json::from_msgpack(response.data, response.data + response.size);
    ffi::free_response(response);

    return object.template get<T>();
}

BowModel load_model(const std::string& path, bool& converted) {
    Response response = ffi::load_model(path.c_str(), &converted);
    return parse_response<BowModel>(response);
}

void save_model(const BowModel& model, const std::string& path, bool backup) {
    std::vector<uint8_t> data = json::to_msgpack(model);
    Response response = ffi::save_model(data.data(), data.size(), path.c_str(), backup);
    check_response(response);
}

BowResult load_result(const std::string& path) {
    Response response = ffi::load_result(path.c_str());
    return parse_response<BowResult>(response);
}

void save_result(const BowResult& result, const std::string& path) {
    std::vector<uint8_t> data = json::to_msgpack(result);
    Response response = ffi::save_result(data.data(), data.size(), path.c_str());
    check_response(response);
}

LimbInfo compute_geometry(const BowModel& model) {
    std::vector<uint8_t> data = json::to_msgpack(model);
    Response response = ffi::compute_geometry(data.data(), data.size());
    return parse_response<LimbInfo>(response);
}

BowResult simulate_model(const BowModel& model, Mode mode, std::function<bool(Mode, double)> callback) {
    std::vector<uint8_t> data = json::to_msgpack(model);
    Response response = ffi::simulate_model(data.data(), data.size(), mode, [](Mode mode, double progress, void* userdata) {
        auto callback = static_cast<std::function<bool(Mode, double)>*>(userdata);    // Pass in lambda, including captured environment, as userdata
        return (*callback)(mode, progress);
    }, &callback);
    return parse_response<BowResult>(response);
}
