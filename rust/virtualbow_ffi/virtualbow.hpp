#pragma once

/* Generated with cbindgen:0.28.0 */

#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

namespace ffi {

enum class Mode {
    Static,
    Dynamic,
};

struct Response {
    char *error;
    uint8_t *data;
    uintptr_t size;
};

extern "C" {

Response new_model();

Response load_model(const char *path, bool convert);

Response save_model(const uint8_t *data, uintptr_t size, const char *path);

Response load_result(const char *path);

Response save_result(const uint8_t *data, uintptr_t size, const char *path);

Response compute_geometry(const uint8_t *data, uintptr_t size);

Response simulate_model(const uint8_t *data, uintptr_t size, Mode mode, bool (*callback)(Mode, double));

void free_response(Response response);

}  // extern "C"

}  // namespace ffi
