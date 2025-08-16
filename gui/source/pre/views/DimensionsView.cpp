#include "DimensionsView.hpp"
#include "primitive/DoubleView.hpp"
#include "primitive/EnumSelectionView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/models/DimensionsModel.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "solver/BowModel.hpp"

DimensionsView::DimensionsView(DimensionsModel* model) {
    addHeading("Handle");

    addProperty(
        "Reference",
        new EnumSelectionView(model, model->HANDLE_REFERENCE,
                              QList<EnumItem<HandleReference>>{
                                  {HandleReference::Back, "Back", "Measure from the back of the bow"},
                                  {HandleReference::Belly, "Belly", "Measure from the belly of the bow"},
                                  {HandleReference::Profile, "Profile", "Measure from the profile curve"}
                              },
                              "Reference point at the limb base from which the handle dimensions as well as the brace height and draw length are measured"
                              )
        );

    addProperty(
        "Length",
        new DoubleView(model, model->HANDLE_LENGTH, Quantities::length, DoubleRange::nonNegative(1e-3), "Length of the handle, i.e. distance between the upper and lower limb as measured by the reference points")
        );

    addProperty(
        "Offset",
        new DoubleView(model, model->HANDLE_OFFSET, Quantities::length, DoubleRange::unrestricted(1e-3), "Offset of the handle's pivot point in the direction of draw")
        );

    addProperty(
        "Angle",
        new DoubleView(model, model->HANDLE_ANGLE, Quantities::angle, DoubleRange::unrestricted(0.25*M_PI/180), "Angle at which the limbs are attached to the handle")
        );

    addHeading("Draw");

    addProperty(
        "Brace height",
        new DoubleView(model, model->BRACE_HEIGHT, Quantities::length, DoubleRange::positive(1e-3), "Brace height of the bow, i.e. distance between string and the handle's pivot point in the braced state")
    );

    addProperty(
        "Draw length",
        new DoubleView(model, model->DRAW_LENGTH, Quantities::length, DoubleRange::positive(1e-3), "Draw length of the bow, i.e. distance between string and the handle's pivot point in the fully drawn state")
    );

    addStretch();
}
