#pragma once
#include "solver/Input.hpp"
#include "solver/Output.hpp"
#include <virtualbow.hpp>

// C++ wrappers around solver's C interface

class SolverException: public std::exception {
    std::string message;

public:
    SolverException(std::string message)
        : message(message) {

    }

    const char* what() const noexcept override {
        return message.c_str();
    }
};

using Mode = ffi::Mode;

BowModel new_model();

BowModel load_model(const std::string& path, bool convert);

void save_model(const BowModel& model, const std::string& path);

BowResult load_result(const std::string& path);

void save_result(const BowResult& result, const std::string& path);

BowResult simulate_model(const BowModel& model, Mode mode, bool (*callback)(Mode, double));
