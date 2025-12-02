#include "DimensionsView.hpp"
#include "primitive/DoubleView.hpp"
#include "primitive/EnumSelectionView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/models/DimensionsModel.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "solver/BowModel.hpp"
#include "pre/Language.hpp"

DimensionsView::DimensionsView(DimensionsModel* model) {
    addHeading("Handle");
    addProperty("Reference", new EnumSelectionView(model, model->HANDLE_REFERENCE,
                              QList<EnumItem<HandleReference>>{
                                  {HandleReference::Back, "Back", Tooltips::HandleReferenceBack},
                                  {HandleReference::Belly, "Belly", Tooltips::HandleReferenceBelly},
                                  {HandleReference::Profile, "Profile", Tooltips::HandleReferenceProfile}
                              },
            Tooltips::HandleReference
    ));
    addProperty("Length", new DoubleView(model, model->HANDLE_LENGTH, Quantities::length, DoubleRange::nonNegative(1e-3), Tooltips::HandleLength));
    addProperty("Offset", new DoubleView(model, model->HANDLE_OFFSET, Quantities::length, DoubleRange::unrestricted(1e-3), Tooltips::HandleOffset));
    addProperty("Angle", new DoubleView(model, model->HANDLE_ANGLE, Quantities::angle, DoubleRange::unrestricted(0.25*M_PI/180), Tooltips::HandleAngle));

    addHeading("Draw");
    addProperty("Brace height", new DoubleView(model, model->BRACE_HEIGHT, Quantities::length, DoubleRange::positive(1e-3), Tooltips::BraceHeight));
    addProperty("Draw length", new DoubleView(model, model->DRAW_LENGTH, Quantities::length, DoubleRange::positive(1e-3), Tooltips::DrawLength));

    addStretch();
}
