#include "StringView.hpp"
#include "pre/models/StringModel.hpp"
#include "primitive/DoubleView.hpp"
#include "primitive/IntegerView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/utils/IntegerRange.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "pre/Language.hpp"

StringView::StringView(StringModel* model) {
    addProperty("Strand stiffness", new DoubleView(model, model->STRAND_STIFFNESS, Quantities::linear_stiffness, DoubleRange::positive(1.0), Tooltips::StringLinearStiffness));
    addProperty("Strand density", new DoubleView(model, model->STRAND_DENSITY, Quantities::linear_density, DoubleRange::positive(1e-4), Tooltips::StringLinearDensity));
    addProperty("Num. Strands", new IntegerView(model, model->N_STRANDS, IntegerRange::positive(), Tooltips::StringNumStrands));
    addStretch();
}
