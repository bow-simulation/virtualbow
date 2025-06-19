#include "ArcView.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "pre/models/ArcModel.hpp"

ArcView::ArcView(ArcModel* model) {
    addProperty(
        "Length",
        "Length of the arc segment",
        new DoubleView(model, model->LENGTH, Quantities::length, DoubleRange::positive(1e-3))
    );

    addProperty(
        "Radius",
        "Radius of the arc segment",
        new DoubleView(model, model->LENGTH, Quantities::length, DoubleRange::unrestricted(1e-3))
    );

    addStretch();
}
