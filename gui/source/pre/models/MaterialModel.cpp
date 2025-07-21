#include "MaterialModel.hpp"
#include "solver/BowModel.hpp"

MaterialModel::MaterialModel(Material& material) {
    NAME = addString(material.name);
    COLOR = addColor(material.color);
    DENSITY = addDouble(material.density);
    YOUNGS_MODULUS = addDouble(material.youngs_modulus);
    SHEAR_MODULUS = addDouble(material.shear_modulus);
}
