#pragma once

/* Generated with cbindgen:0.29.2 */

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
    uintptr_t capacity;
};

extern "C" {

Response load_model(const char *path, bool *converted);

Response save_model(const uint8_t *data, uintptr_t size, const char *path, bool backup);

Response load_result(const char *path);

Response save_result(const uint8_t *data, uintptr_t size, const char *path);

Response compute_geometry(const uint8_t *data, uintptr_t size);

Response simulate_model(const uint8_t *data,
                        uintptr_t size,
                        Mode mode,
                        bool (*callback)(Mode, double, void*),
                        void *userdata);

void free_response(Response response);

}  // extern "C"

}  // namespace ffi
