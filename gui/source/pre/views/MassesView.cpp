#include "MassesView.hpp"
#include "pre/viewmodels/MassesVM.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"

MassesView::MassesView(MassesVM* model) {
    addProperty(
        "Arrow",
        "Mass of the arrow",
        new DoubleView(model, model->ARROW, Quantities::mass, DoubleRange::positive(0.1e-3))
    );

    addProperty(
        "String center",
        "Additional mass(es) at the string center (e.g. serving, nocking point)",
        new DoubleView(model, model->STRING_CENTER, Quantities::mass, DoubleRange::nonNegative(0.1e-3))
    );

    addProperty(
        "String tip",
        "Additional mass(es) at the string tip (e.g. serving)",
        new DoubleView(model, model->STRING_TIP, Quantities::mass, DoubleRange::nonNegative(0.1e-3))
    );

    addProperty(
        "Limb tip",
        "Additional mass(es) at the limb tip (e.g. tip overlay)",
        new DoubleView(model, model->LIMB_TIP, Quantities::mass, DoubleRange::nonNegative(0.1e-3))
    );

    addStretch();
}
