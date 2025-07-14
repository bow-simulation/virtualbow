#include "StringView.hpp"
#include "pre/models/StringModel.hpp"
#include "primitive/DoubleView.hpp"
#include "primitive/IntegerView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/utils/IntegerRange.hpp"
#include "pre/models/units/UnitSystem.hpp"

StringView::StringView(StringModel* model) {
    addProperty(
        "Strand stiffness",
        new DoubleView(model, model->STRAND_STIFFNESS, Quantities::linear_stiffness, DoubleRange::positive(1.0), "Linear stiffness of a single strand of the string material")
    );

    addProperty(
        "Strand density",
        new DoubleView(model, model->STRAND_DENSITY, Quantities::linear_density, DoubleRange::positive(1e-4), "Linear density of a single strand of the string material")
    );

    addProperty(
        "Num. Strands",
        new IntegerView(model, model->N_STRANDS, IntegerRange::positive(), "Total number of strands in the string")
    );

    addStretch();
}
