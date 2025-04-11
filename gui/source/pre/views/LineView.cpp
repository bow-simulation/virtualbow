#include "LineView.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"
#include "pre/viewmodels/LineVM.hpp"

LineView::LineView(LineVM* model) {
    addProperty(
        "Length",
        "Length of the line segment",
        new DoubleView(model, model->LENGTH, Quantities::length, DoubleRange::positive(0.1e-3))
    );

    addStretch();
}
