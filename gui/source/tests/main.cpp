#define CATCH_CONFIG_MAIN

#include "solver/interface.hpp"
#include "config.hpp"

#include <catch2/catch.hpp>

// Location of the solver's test data directory
const std::string TEST_DATA_DIR = std::string(Config::CMAKE_SOURCE_DIR) + "/../rust/virtualbow/data";

TEST_CASE("load-model-file") {
    REQUIRE_NOTHROW(load_model(TEST_DATA_DIR + "/input/valid_model.bow", false));
    REQUIRE_THROWS(load_model(TEST_DATA_DIR + "/input/nonexistent.bow", false));
}

TEST_CASE("save-model-file") {
    BowModel model = new_model();
    REQUIRE_NOTHROW(save_model(model, TEST_DATA_DIR + "/temp/model.bow"));
    REQUIRE_THROWS(save_model(model, TEST_DATA_DIR + "/temp/nonexistent/model.bow"));
}

TEST_CASE("load-result-file") {
    REQUIRE_NOTHROW(load_result(TEST_DATA_DIR + "/output/valid_results.res"));
    REQUIRE_THROWS(load_result(TEST_DATA_DIR + "/output/nonexistent.res"));
}

TEST_CASE("save-result-file") {
    BowResult result;
    REQUIRE_NOTHROW(save_result(result, TEST_DATA_DIR + "/temp/result.res"));
    REQUIRE_THROWS(save_result(result, TEST_DATA_DIR + "/temp/nonexistent/result.res"));
}
