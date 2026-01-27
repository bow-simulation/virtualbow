#pragma once
#include "pre/models/PropertyListModel.hpp"

struct Settings;

class SettingsModel: public PropertyListModel {
public:
    QPersistentModelIndex N_LIMB_ELEMENTS;
    QPersistentModelIndex N_EVAL_POINTS;
    QPersistentModelIndex MIN_DRAW_RESOLUTION;
    QPersistentModelIndex MAX_DRAW_RESOLUTION;
    QPersistentModelIndex STATIC_ITERATION_TOLERANCE;
    QPersistentModelIndex ARROW_CLAMP_FORCE;
    QPersistentModelIndex STRING_COMPRESSION_FACTOR;
    QPersistentModelIndex TIMESPAN_FACTOR;
    QPersistentModelIndex TIMEOUT_FACTOR;
    QPersistentModelIndex MIN_TIMESTEP;
    QPersistentModelIndex MAX_TIMESTEP;
    QPersistentModelIndex STEPS_PER_PERIOD;
    QPersistentModelIndex DYNAMIC_ITERATION_TOLERANCE;

    SettingsModel(Settings& settings);
};
