#include "SettingsView.hpp"
#include "primitive/IntegerView.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/IntegerRange.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "pre/models/SettingsModel.hpp"

SettingsView::SettingsView(SettingsModel* model) {
    addHeading("General");

    addProperty(
        "Limb elements",
        new IntegerView(model, model->N_LIMB_ELEMENTS, IntegerRange::positive(), "Number of finite elements used to approximate the limb")
    );

    addProperty(
        "Limb eval points",
        new IntegerView(model, model->N_EVAL_POINTS, IntegerRange::greaterOrEqual(2), "Number of points along the limb where results are evaluated")
    );

    addHeading("Statics");

    addProperty(
        "Min. draw resolution",
        new IntegerView(model, model->MIN_DRAW_RESOLUTION, IntegerRange::positive(), "Lower bound for the number of equilibrium points from brace height to full draw")
    );

    addProperty(
        "Max. draw resolution",
        new IntegerView(model, model->MAX_DRAW_RESOLUTION, IntegerRange::positive(), "Upper bound for the number of equilibrium points from brace height to full draw")
    );

    addHeading("Dynamics");

    addProperty(
        "Arrow clamp force",
        new DoubleView(model, model->ARROW_CLAMP_FORCE, Quantities::force, DoubleRange::nonNegative(0.1), "Force that the arrow has to overcome before separating from the string")
    );

    addProperty(
        "String compression factor",
        new DoubleView(model, model->STRING_COMPRESSION_FACTOR, Quantities::ratio, DoubleRange::positive(1e-6), "Factor for the compressive stiffness of the string relative to its tensile stiffness.\nShould be very low but can't be zero for numerical reasons.")
    );

    addProperty(
        "Timespan factor",
        new DoubleView(model, model->TIMESPAN_FACTOR, Quantities::ratio, DoubleRange::positive(1e-2), "Factor for controlling the end time of the dynamic simulation.\nThe value 1.0 corresponds to the time until the arrow reaches brace height.\nValues larger than 1.0 extend the simulated time beyond that.")
    );

    addProperty(
        "Timeout factor",
        new DoubleView(model, model->TIMEOUT_FACTOR, Quantities::ratio, DoubleRange::positive(0.1),         "Factor for controlling the timeout of the dynamic simulation.\nThe simulation is aborted when arrow separation didn't happen until the simulation time exceeds the timeout factor multiplied by a characteristic time of the bow.")
    );

    addProperty(
        "Min. timestep",
        new DoubleView(model, model->MIN_TIMESTEP, Quantities::time, DoubleRange::positive(1e-6), "Lower bound for the time step of the dynamic simulation")
    );

    addProperty(
        "Max. timestep",
        new DoubleView(model, model->MAX_TIMESTEP, Quantities::time, DoubleRange::positive(1e-4), "Upper bound for the time step of the dynamic simulation")
    );

    addProperty(
        "Steps per period",
        new IntegerView(model, model->STEPS_PER_PERIOD, IntegerRange::positive(), "Number of steps the dynamic solver tries to take per current characteristic time period of the system")
    );

    addStretch();
}
