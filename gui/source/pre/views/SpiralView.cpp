#include "SpiralView.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "pre/models/SpiralModel.hpp"

SpiralView::SpiralView(SpiralModel* model) {
    addProperty(
        "Length",
        new DoubleView(model, model->LENGTH, Quantities::length, DoubleRange::positive(1e-3), "Length of the spiral segment")
    );

    addProperty(
        "Radius (start)",
        new DoubleView(model, model->RADIUS_START, Quantities::length, DoubleRange::unrestricted(1e-3), "Start radius of the spiral segment")
    );

    addProperty(
        "Radius (end)",
        new DoubleView(model, model->RADIUS_END, Quantities::length, DoubleRange::unrestricted(1e-3), "End radius of the spiral segment")
    );

    addStretch();
}
