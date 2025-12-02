#include "SpiralView.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "pre/models/SpiralModel.hpp"
#include "pre/Language.hpp"

SpiralView::SpiralView(SpiralModel* model) {
    addProperty("Length", new DoubleView(model, model->LENGTH, Quantities::length, DoubleRange::positive(1e-3), Tooltips::SpiralSegmentLength));
    addProperty("Radius (start)", new DoubleView(model, model->RADIUS_START, Quantities::length, DoubleRange::unrestricted(1e-3), Tooltips::SpiralSegmentRadiusStart));
    addProperty("Radius (end)", new DoubleView(model, model->RADIUS_END, Quantities::length, DoubleRange::unrestricted(1e-3), Tooltips::SpiralSegmentRadiusEnd));
    addStretch();
}
