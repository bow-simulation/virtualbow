#include "DampingView.hpp"
#include "pre/models/DampingModel.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "pre/Language.hpp"

DampingView::DampingView(DampingModel* model) {
    addProperty("Limb damping ratio", new DoubleView(model, model->DAMPING_RATIO_LIMBS, Quantities::ratio, DoubleRange::inclusive(0.0, 1.0, 1e-2), Tooltips::DampingRatioLimbs));
    addProperty("String damping ratio", new DoubleView(model, model->DAMPING_RATIO_STRING, Quantities::ratio, DoubleRange::inclusive(0.0, 1.0, 1e-2), Tooltips::DampingRatioString));
    addStretch();
}
