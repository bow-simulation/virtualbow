#include "MaterialView.hpp"
#include "primitive/ColorView.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/models/MaterialModel.hpp"
#include "pre/models/units/UnitSystem.hpp"

MaterialView::MaterialView(MaterialModel* model) {
    addProperty(
        "Color",
        new ColorView(model, model->COLOR, "Color of the material, only used for visualization")
    );

    addProperty(
        "Density",
        new DoubleView(model, model->DENSITY, Quantities::density, DoubleRange::positive(1.0), "Density of the material (mass per unit volume)")
    );

    addHeading("Stiffness");

    addProperty(
        "Young's modulus",
        new DoubleView(model, model->YOUNGS_MODULUS, Quantities::elastic_modulus, DoubleRange::positive(0.1e9), "Young's modulus of the material")
    );

    addProperty(
        "Shear modulus",
        new DoubleView(model, model->SHEAR_MODULUS, Quantities::elastic_modulus, DoubleRange::positive(0.1e9), "Shear modulus of the material")
    );

    addHeading("Strength");

    addProperty(
        "Tensile strength",
        new DoubleView(model, model->TENSILE_STRENGTH, Quantities::stress, DoubleRange::nonNegative(0.1e6), "Tensile strength of the material")
    );

    addProperty(
        "Compressive strength",
        new DoubleView(model, model->COMPRESSIVE_STRENGTH, Quantities::stress, DoubleRange::nonNegative(0.1e6), "Compressive strength of the material")
    );

    addProperty(
        "Safety margin",
        new DoubleView(model, model->SAFETY_MARGIN, Quantities::ratio, DoubleRange::nonNegative(1e-2), "Safety margin for tensile and compressive strength")
    );

    addStretch();
}
