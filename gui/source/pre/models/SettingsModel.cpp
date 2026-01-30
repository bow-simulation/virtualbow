#include "SettingsModel.hpp"
#include "solver/BowModel.hpp"

SettingsModel::SettingsModel(Settings& settings) {
    N_LIMB_ELEMENTS = addInteger(settings.num_limb_elements);
    N_EVAL_POINTS = addInteger(settings.num_limb_eval_points);
    MIN_DRAW_RESOLUTION = addInteger(settings.min_draw_resolution);
    MAX_DRAW_RESOLUTION = addInteger(settings.max_draw_resolution);
    STATIC_ITERATION_TOLERANCE = addDouble(settings.static_iteration_tolerance);
    ARROW_CLAMP_FORCE = addDouble(settings.arrow_clamp_force);
    STRING_COMPRESSION_FACTOR = addDouble(settings.string_compression_factor);
    TIMESPAN_FACTOR = addDouble(settings.timespan_factor);
    TIMEOUT_FACTOR = addDouble(settings.timeout_factor);
    MIN_TIMESTEP = addDouble(settings.min_timestep);
    MAX_TIMESTEP = addDouble(settings.max_timestep);
    STEPS_PER_PERIOD = addInteger(settings.steps_per_period);
    DYNAMIC_ITERATION_TOLERANCE = addDouble(settings.dynamic_iteration_tolerance);
}
