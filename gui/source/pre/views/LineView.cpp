#include "LineView.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "pre/models/LineModel.hpp"
#include "pre/Language.hpp"

LineView::LineView(LineModel* model) {
    addProperty("Length", new DoubleView(model, model->LENGTH, Quantities::length, DoubleRange::positive(1e-3), Tooltips::LineSegmentLength));
    addStretch();
}
