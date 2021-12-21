#pragma once
#include <nlohmann/json.hpp>

using nlohmann::json;

class Conversion {
public:
    static void to_current(json& obj);

private:
    static void convert_0_7_1_to_0_8_0(json& obj);

    static void convert_0_7_0_to_0_7_1(json& obj);

    static void convert_0_6_1_to_0_7_0(json& obj);

    static void convert_0_6_0_to_0_6_1(json& obj);

    static void convert_0_5_0_to_0_6_0(json& obj);

    static void convert_0_4_0_to_0_5_0(json& obj);

    static void convert_0_3_0_to_0_4_0(json& obj);

    static void convert_0_2_0_to_0_3_0(json& obj);

    static void convert_0_1_0_to_0_2_0(json& obj);
};
