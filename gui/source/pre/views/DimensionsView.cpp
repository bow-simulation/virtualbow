#include "DimensionsView.hpp"
#include "primitive/DoubleView.hpp"
#include "primitive/EnumSelectionView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/viewmodels/DimensionsVM.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"

DimensionsView::DimensionsView(DimensionsVM* model) {
    addHeading("Draw");

    addProperty(
        "Brace height",
        "Brace height of the bow, i.e. distance between string and the handle origin in the braced state",
        new DoubleView(model, model->BRACE_HEIGHT, Quantities::length, DoubleRange::positive(1e-3))
    );

    addProperty(
        "Draw length",
        "Draw length of the bow, i.e. distance between string and the handle origin in the fully drawn state",
        new DoubleView(model, model->DRAW_LENGTH, Quantities::length, DoubleRange::positive(1e-3))
    );

    addHeading("Handle");

    addProperty(
        "Reference",
        "Reference point at the limb base from which the handle dimensions are measured",
        new EnumSelectionView(model, model->HANDLE_ORIGIN, {"Back", "Belly", "Profile"}, QList<HandleOrigin>{HandleOrigin::Back, HandleOrigin::Belly, HandleOrigin::Profile})
    );

    addProperty(
        "Length",
        "Length of the handle, i.e. distance between the upper and lower limb as measured by the reference points",
        new DoubleView(model, model->HANDLE_LENGTH, Quantities::length, DoubleRange::nonNegative(1e-3))
    );

    addProperty(
        "Offset",
        "Offset of the handle's pivot point in the direction of draw",
        new DoubleView(model, model->HANDLE_OFFSET, Quantities::length, DoubleRange::unrestricted(1e-3))
    );

    addProperty(
        "Angle",
        "Angle at which the limbs are attached to the handle",
        new DoubleView(model, model->HANDLE_ANGLE, Quantities::angle, DoubleRange::unrestricted(1e-3))
    );

    addStretch();
}
