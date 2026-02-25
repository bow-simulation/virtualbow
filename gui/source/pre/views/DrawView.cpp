#include "DrawView.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "DrawLengthView.hpp"
#include "pre/models/DrawModel.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "pre/Language.hpp"

DrawView::DrawView(DrawModel* model) {
    addProperty("Brace height", new DoubleView(model, model->BRACE_HEIGHT, Quantities::length, DoubleRange::positive(1e-3), Tooltips::BraceHeight));
    addProperty("Draw length", new DrawLengthView(model, model->DRAW_LENGTH));
    addStretch();
}
