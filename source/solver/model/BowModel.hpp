#pragma once
#include "solver/model/input/InputData.hpp"
#include "solver/model/output/OutputData.hpp"
#include "solver/fem/System.hpp"
#include <functional>

enum class SimulationMode {
    Static,
    Dynamic
};

class BowModel {
public:
    using Callback = std::function<void(int, int)>;    // Progress (static, dynamic) in percent
    static OutputData simulate(const InputData& input, SimulationMode mode, const Callback& callback);

private:
    BowModel(const InputData& input);
    void init_limb(const Callback& callback, SetupData& output);
    void init_string(const Callback& callback, SetupData& output);
    void init_masses(const Callback& callback, SetupData& output);

    SetupData simulate_setup(const Callback& callback);
    BowStates simulate_statics(const Callback& callback);
    BowStates simulate_dynamics(const Callback& callback);

    void add_state(BowStates& states) const;

private:
    const InputData& input;

    System system;
    std::vector<Node> nodes_limb;
    std::vector<Node> nodes_string;
    Node node_arrow;
};
