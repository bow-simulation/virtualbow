#pragma once
#include "solver/BowModel.hpp"
#include "solver/BowResult.hpp"
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

BowModel load_model(const std::string& path, bool& converted);

void save_model(const BowModel& model, const std::string& path, bool backup);

BowResult load_result(const std::string& path);

void save_result(const BowResult& result, const std::string& path);

LimbInfo compute_geometry(const BowModel& model);

BowResult simulate_model(const BowModel& model, Mode mode, std::function<bool(Mode, double)> callback);
