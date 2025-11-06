#include "MassesView.hpp"
#include "ArrowMassView.hpp"
#include "pre/models/MassesModel.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/models/units/UnitSystem.hpp"

MassesView::MassesView(MassesModel* model) {
    addProperty(
        "Arrow",
        new ArrowMassView(model, model->ARROW)
    );

    addProperty(
        "String center",
        new DoubleView(model, model->STRING_CENTER, Quantities::mass, DoubleRange::nonNegative(1e-3), "Additional mass(es) at the string center (e.g. serving, nocking point)")
    );

    addProperty(
        "String tip",
        new DoubleView(model, model->STRING_TIP, Quantities::mass, DoubleRange::nonNegative(1e-3), "Additional mass(es) at the string tip (e.g. serving)")
    );

    addProperty(
        "Limb tip",
        new DoubleView(model, model->LIMB_TIP, Quantities::mass, DoubleRange::nonNegative(1e-3), "Additional mass(es) at the limb tip (e.g. tip overlay)")
    );

    addStretch();
}
