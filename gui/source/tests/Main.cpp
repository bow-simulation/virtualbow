#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "solver/model/input/InputData.hpp"
#include "solver/model/output/OutputData.hpp"
#include "config.hpp"

// Location of the solver's test data directory
const std::string TEST_DATA_DIR = std::string(Config::CMAKE_SOURCE_DIR) + "/../rust/virtualbow/data";

// Tests if a model file from the solver's test data can be loaded successfully
TEST_CASE("load-model-file") {
    REQUIRE_NOTHROW(InputData(TEST_DATA_DIR + "/input/valid_model.bow"));
}

// Tests if a result file from the solver's test data can be loaded successfully
TEST_CASE("load-result-file") {
    REQUIRE_NOTHROW(OutputData(TEST_DATA_DIR + "/output/valid_results.res"));
}
