#include "OutputData.hpp"

BowStates::BowStates()
{

}

StaticData::StaticData(BowStates states)
    : states(states),
      final_draw_force(states.draw_force.back()),
      drawing_work(states.e_pot_limbs.back() + states.e_pot_string.back() - states.e_pot_limbs.front() - states.e_pot_string.front()),
      storage_ratio(drawing_work/(0.5*(states.draw_length.back() - states.draw_length.front())*final_draw_force))
{

}

DynamicData::DynamicData(BowStates states, const StaticData& static_data)
    : states(states),
      final_arrow_velocity(std::abs(states.vel_arrow.back())),
      final_arrow_energy(states.e_kin_arrow.back()),
      efficiency(final_arrow_energy/static_data.drawing_work)
{

}
