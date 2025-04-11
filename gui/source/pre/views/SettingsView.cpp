#include "SettingsView.hpp"
#include "primitive/IntegerView.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/IntegerRange.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"
#include "pre/viewmodels/SettingsVM.hpp"

SettingsView::SettingsView(SettingsVM* model) {
    addHeading("General");

    addProperty(
        "Limb elements",
        "Number of finite elements used to approximate the limb"
        "<ul>"
        "<li>Larger numbers increase accuracy but make the simulation slower</li>"
        "<li>Smaller numbers speed up the simulation but reduce accuracy</li>"
        "</ul>",
        new IntegerView(model, model->N_LIMB_ELEMENTS, IntegerRange::positive())
    );

    addProperty(
        "Limb eval points",
        "Number of points along the limb for evaluating results"
        "<ul>"
        "<li>Larger numbers increase the resolution of the output</li>"
        "<li>Smaller numbers decrease the resolution of the output</li>"
        "</ul>",
        new IntegerView(model, model->N_EVAL_POINTS, IntegerRange::positive())
    );

    addHeading("Statics");

    addProperty(
        "Min. draw resolution",
        "Lower bound for the number of equilibrium points from brace height to full draw",
        new IntegerView(model, model->MIN_DRAW_RESOLUTION, IntegerRange::positive())
    );

    addProperty(
        "Max. draw resolution",
        "Upper bound for the number of equilibrium points from brace height to full draw",
        new IntegerView(model, model->MAX_DRAW_RESOLUTION, IntegerRange::positive())
    );

    addHeading("Dynamics");

    addProperty(
        "Arrow clamp force",
        "Force that the arrow has to overcome before separating from the string",
        new DoubleView(model, model->ARROW_CLAMP_FORCE, Quantities::force, DoubleRange::nonNegative(1e-2))
    );

    addProperty(
        "String compression factor",
        "Factor for the compressive stiffness of the string relative to its tensile stiffness.\nShould be very low but can't be zero for numerical reasons.",
        new DoubleView(model, model->STRING_COMPRESSION_FACTOR, Quantities::ratio, DoubleRange::positive(1e-6))
    );

    addProperty(
        "Timespan factor",
        "Factor for controlling the end time of the dynamic simulation.\nThe value 1.0 corresponds to the time until the arrow reaches brace height.\nValues larger than 1.0 extend the simulated time beyond that.",
        new DoubleView(model, model->TIMESPAN_FACTOR, Quantities::ratio, DoubleRange::positive(1e-2))
    );

    addProperty(
        "Timeout factor",
        "Factor for controlling the timeout of the dynamic simulation.\nThe simulation is aborted when arrow separation didn't happen until the simulation time exceeds the timeout factor multiplied by a characteristic time of the bow.",
        new DoubleView(model, model->TIMEOUT_FACTOR, Quantities::ratio, DoubleRange::positive(1e-2))
    );

    addProperty(
        "Min. timestep",
        "Lower bound for the time step of the dynamic simulation",
        new DoubleView(model, model->MIN_TIMESTEP, Quantities::ratio, DoubleRange::positive(1e-6))
    );

    addProperty(
        "Max. timestep",
        "Upper bound for the time step of the dynamic simulation",
        new DoubleView(model, model->MAX_TIMESTEP, Quantities::ratio, DoubleRange::positive(1e-4))
    );

    addProperty(
        "Steps per period",
        "Number of steps the dynamic solver tries to take per current characteristic time period of the system",
        new IntegerView(model, model->STEPS_PER_PERIOD, IntegerRange::positive())
    );

    addStretch();
}
