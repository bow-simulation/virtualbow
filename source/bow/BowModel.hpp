#pragma once
#include "bow/input/InputData.hpp"
#include "bow/output/OutputData.hpp"
#include "fem/System.hpp"
#include <functional>

class BowModel
{
public:
    using Callback = std::function<bool(int)>;    // Progress (percent) -> Continue (true/false)
    static OutputData run_static_simulation(const InputData& input, const Callback& callback);
    static OutputData run_dynamic_simulation(const InputData& input, const Callback& callback1, const Callback& callback2);

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
