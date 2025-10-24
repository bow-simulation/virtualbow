#define CATCH_CONFIG_MAIN

#include "solver/API.hpp"
#include <catch2/catch.hpp>
#include <filesystem>

namespace fs = std::filesystem;

TEST_CASE("test-solver-api") {
    std::string model_file = (fs::temp_directory_path() /= "model.bow").string();      // Temporary model file path
    std::string result_file = (fs::temp_directory_path() /= "result.res").string();    // Temporary result file path

    // Create new default bow model
    BowModel model = new_model();

    // Compute model geometry
    LimbInfo geometry = compute_geometry(model);

    // Save model to file
    save_model(model, model_file, false);

    // Load model from file
    bool converted;
    model = load_model(model_file, converted);

    // Run a full simulation
    BowResult result = simulate_model(model, Mode::Dynamic, [](Mode mode, double progress) {
        INFO("Mode: " << (int) mode << ", Progress: " << progress << "%\n");
        return true;
    });

    // Save result to file
    save_result(result, result_file);

    // Load result from file
    result = load_result(result_file);
}
