#pragma once

/* Generated with cbindgen:0.28.0 */

#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

struct Buffer {
    uint8_t *data;
    uintptr_t size;
};

extern "C" {

Buffer load_model(const int8_t *path, bool convert, const int8_t *error);

void free_buffer(Buffer buffer);

}  // extern "C"
