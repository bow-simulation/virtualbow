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

    addProperty(
        "Young's modulus",
        new DoubleView(model, model->YOUNGS_MODULUS, Quantities::elastic_modulus, DoubleRange::positive(1e8), "Young's modulus of the material")
    );

    addProperty(
        "Shear modulus",
        new DoubleView(model, model->SHEAR_MODULUS, Quantities::elastic_modulus, DoubleRange::positive(1e8), "Shear modulus of the material")
    );

    addStretch();
}
