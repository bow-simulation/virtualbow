#pragma once
#include "DiscreteLimb.hpp"
#include <memory>

struct BowSetup
{
    DiscreteLimb limb;
    double string_length;
};

struct BowStates
{
    std::vector<double> time;
    std::vector<double> draw_force;
    std::vector<double> pos_string_center;
    std::vector<double> pos_arrow;

    std::vector<std::vector<double>> pos_limb_x;
    std::vector<std::vector<double>> pos_limb_y;
    std::vector<std::vector<double>> pos_string_x;
    std::vector<std::vector<double>> pos_string_y;

    /*
    pub time: Vec<f64>,
    pub draw_force: Vec<f64>,

    pub pos_limb: Vec<Vec<[f64; 2]>>,
    pub vel_limb: Vec<Vec<[f64; 2]>>,
    pub acc_limb: Vec<Vec<[f64; 2]>>,

    pub pos_string: Vec<Vec<[f64; 2]>>,
    pub vel_string: Vec<Vec<[f64; 2]>>,
    pub acc_string: Vec<Vec<[f64; 2]>>,
    */
    BowStates()
    {

    }
};

struct OutputData
{
    BowSetup setup;
    std::unique_ptr<BowStates> statics = nullptr;   // Todo: Use boost optional?
    std::unique_ptr<BowStates> dynamics = nullptr;
};
