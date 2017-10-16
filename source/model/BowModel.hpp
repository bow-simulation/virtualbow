#pragma once
#include "model/InputData.hpp"
#include "model/output/OutputData.hpp"
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

    void init_limb();
    void init_string();
    void init_masses();

    void simulate_statics(const Callback& callback);
    void simulate_dynamics(const Callback& callback);
    void add_state(BowStates& states) const;

private:
    const InputData& input;
    OutputData output;

    System system;
    std::vector<Node> nodes_limb;
    std::vector<Node> nodes_string;
    Node node_arrow;
};
