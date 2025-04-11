#include "SpiralView.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"
#include "pre/viewmodels/SpiralVM.hpp"

SpiralView::SpiralView(SpiralVM* model) {
    addProperty(
        "Length",
        "Length of the spiral segment",
        new DoubleView(model, model->LENGTH, Quantities::length, DoubleRange::positive(0.1e-3))
    );

    addProperty(
        "Radius (start)",
        "Start radius of the spiral segment",
        new DoubleView(model, model->RADIUS_START, Quantities::length, DoubleRange::unrestricted(0.1e-3))
    );

    addProperty(
        "Radius (end)",
        "End radius of the spiral segment",
        new DoubleView(model, model->RADIUS_END, Quantities::length, DoubleRange::unrestricted(0.1e-3))
    );

    addStretch();
}
