#include "ArcView.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"
#include "pre/viewmodels/ArcVM.hpp"

ArcView::ArcView(ArcVM* model) {
    addProperty(
        "Length",
        "Length of the arc segment",
        new DoubleView(model, model->LENGTH, Quantities::length, DoubleRange::positive(0.1e-3))
    );

    addProperty(
        "Radius",
        "Radius of the arc segment",
        new DoubleView(model, model->LENGTH, Quantities::length, DoubleRange::unrestricted(0.1e-3))
    );

    addStretch();
}
