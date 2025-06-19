#include "LineView.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "pre/models/LineModel.hpp"

LineView::LineView(LineModel* model) {
    addProperty(
        "Length",
        "Length of the line segment",
        new DoubleView(model, model->LENGTH, Quantities::length, DoubleRange::positive(1e-3))
    );

    addStretch();
}
