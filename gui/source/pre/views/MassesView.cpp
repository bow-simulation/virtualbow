#include "MassesView.hpp"
#include "ArrowMassView.hpp"
#include "pre/models/MassesModel.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "pre/Language.hpp"

MassesView::MassesView(MassesModel* model) {
    addProperty("Arrow", new ArrowMassView(model, model->ARROW));
    addProperty("String center", new DoubleView(model, model->STRING_CENTER, Quantities::mass, DoubleRange::nonNegative(1e-3), Tooltips::MassStringCenter));
    addProperty("String tip", new DoubleView(model, model->STRING_TIP, Quantities::mass, DoubleRange::nonNegative(1e-3), Tooltips::MassStringTip));
    addProperty("Limb tip", new DoubleView(model, model->LIMB_TIP, Quantities::mass, DoubleRange::nonNegative(1e-3), Tooltips::MassLimbTip));
    addStretch();
}
