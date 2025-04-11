#include "DampingView.hpp"
#include "pre/viewmodels/DampingVM.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"

DampingView::DampingView(DampingVM* model) {
    addProperty(
        "Limb damping ratio",
        "Damping ratio of the limbs",
        new DoubleView(model, model->DAMPING_RATIO_LIMBS, Quantities::ratio, DoubleRange::nonNegative(0.1e-2))
    );

    addProperty(
        "String damping ratio",
        "Damping ratio of the string",
        new DoubleView(model, model->DAMPING_RATIO_STRING, Quantities::ratio, DoubleRange::nonNegative(0.1e-2))
    );

    addStretch();
}
