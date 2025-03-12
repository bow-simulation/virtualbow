#include "interface.hpp"
#include <virtualbow.hpp>
#include <nlohmann/json.hpp>

#include <iostream>

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

BowModel new_model() {
    Response response = ffi::new_model();
    return parse_response<BowModel>(response);
}

BowModel load_model(const std::string& path, bool convert) {
    Response response = ffi::load_model(path.c_str(), convert);
    return parse_response<BowModel>(response);
}

void save_model(const BowModel& model, const std::string& path) {
    std::vector<uint8_t> data = json::to_msgpack(model);
    Response response = ffi::save_model(data.data(), data.size(), path.c_str());
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
