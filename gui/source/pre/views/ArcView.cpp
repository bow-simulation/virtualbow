#include "ArcView.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "pre/models/ArcModel.hpp"
#include "pre/Language.hpp"

ArcView::ArcView(ArcModel* model) {
    addProperty("Length", new DoubleView(model, model->LENGTH, Quantities::length, DoubleRange::positive(1e-3), Tooltips::ArcSegmentLength));
    addProperty("Radius", new DoubleView(model, model->RADIUS, Quantities::length, DoubleRange::unrestricted(1e-3), Tooltips::ArcSegmentRadius));
    addStretch();
}
