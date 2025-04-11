#include "StringView.hpp"
#include "pre/viewmodels/StringVM.hpp"
#include "primitive/DoubleView.hpp"
#include "primitive/IntegerView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/utils/IntegerRange.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"

StringView::StringView(StringVM* model) {
    addProperty(
        "Strand stiffness",
        "Linear stiffness of a single strand of the string material",
        new DoubleView(model, model->STRAND_STIFFNESS, Quantities::linear_stiffness, DoubleRange::positive(1.0))
    );

    addProperty(
        "Strand density",
        "Linear density of a single strand of the string material",
        new DoubleView(model, model->STRAND_DENSITY, Quantities::linear_density, DoubleRange::positive(1.0))
    );

    addProperty(
        "Num. Strands",
        "Total number of strands in the string",
        new IntegerView(model, model->N_STRANDS, IntegerRange::positive())
    );

    addStretch();
}
