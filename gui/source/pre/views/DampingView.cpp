#include "DampingView.hpp"
#include "pre/models/DampingModel.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/models/units/UnitSystem.hpp"

DampingView::DampingView(DampingModel* model) {
    addProperty(
        "Limb damping ratio",
        "Damping ratio of the limbs",
        new DoubleView(model, model->DAMPING_RATIO_LIMBS, Quantities::ratio, DoubleRange::inclusive(0.0, 1.0, 1e-2))
    );

    addProperty(
        "String damping ratio",
        "Damping ratio of the string",
        new DoubleView(model, model->DAMPING_RATIO_STRING, Quantities::ratio, DoubleRange::inclusive(0.0, 1.0, 1e-2))
    );

    addStretch();
}
