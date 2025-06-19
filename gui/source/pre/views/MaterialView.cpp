#include "MaterialView.hpp"
#include "primitive/ColorView.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/models/MaterialModel.hpp"
#include "pre/models/units/UnitSystem.hpp"

MaterialView::MaterialView(MaterialModel* model) {
    addProperty(
        "Color",
        "Color of the material, only used for visualization",
        new ColorView(model, model->COLOR)
    );

    addProperty(
        "Density",
        "Density of the material (mass per unit volume)",
        new DoubleView(model, model->DENSITY, Quantities::density, DoubleRange::positive(1.0))
    );

    addProperty(
        "Young's modulus",
        "Young's modulus of the material",
        new DoubleView(model, model->YOUNGS_MODULUS, Quantities::elastic_modulus, DoubleRange::positive(1e8))
    );

    addProperty(
        "Shear modulus",
        "Shear modulus of the material",
        new DoubleView(model, model->SHEAR_MODULUS, Quantities::elastic_modulus, DoubleRange::positive(1e8))
    );

    addStretch();
}
