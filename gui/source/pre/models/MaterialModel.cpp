#include "MaterialModel.hpp"

MaterialModel::MaterialModel(MainModel *parent, Material& material):
    PropertyListModel(parent)
{
    NAME = addString(material.name);
    COLOR = addColor(material.color);
    DENSITY = addDouble(material.density);
    YOUNGS_MODULUS = addDouble(material.youngs_modulus);
    SHEAR_MODULUS = addDouble(material.shear_modulus);
}
