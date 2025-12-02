#include "MaterialView.hpp"
#include "primitive/ColorView.hpp"
#include "primitive/DoubleView.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/models/MaterialModel.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "pre/Language.hpp"

MaterialView::MaterialView(MaterialModel* model) {
    addProperty("Color", new ColorView(model, model->COLOR, Tooltips::MaterialColor));
    addProperty("Density", new DoubleView(model, model->DENSITY, Quantities::density, DoubleRange::positive(1.0), Tooltips::MaterialDensity));

    addHeading("Stiffness");
    addProperty("Young's modulus", new DoubleView(model, model->YOUNGS_MODULUS, Quantities::elastic_modulus, DoubleRange::positive(0.1e9), Tooltips::MaterialYoungsModulus));
    addProperty("Shear modulus", new DoubleView(model, model->SHEAR_MODULUS, Quantities::elastic_modulus, DoubleRange::positive(0.1e9), Tooltips::MaterialShearModulus));

    addHeading("Strength");
    addProperty("Tensile strength", new DoubleView(model, model->TENSILE_STRENGTH, Quantities::stress, DoubleRange::nonNegative(0.1e6), Tooltips::MaterialTensileStrength));
    addProperty("Compressive strength", new DoubleView(model, model->COMPRESSIVE_STRENGTH, Quantities::stress, DoubleRange::nonNegative(0.1e6), Tooltips::MaterialCompressiveStrength));
    addProperty("Safety margin", new DoubleView(model, model->SAFETY_MARGIN, Quantities::ratio, DoubleRange::nonNegative(1e-2), Tooltips::MaterialSafetyMargin));

    addStretch();
}
