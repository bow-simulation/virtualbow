#include "MaterialModel.hpp"

MaterialModel::MaterialModel(MainModel *parent, Material& material):
    PropertyListModel(parent)
{
    NAME = addString(material.name);
    COLOR = addColor(material.color);
    DENSITY = addDouble(material.rho);
    YOUNGS_MODULUS = addDouble(material.E);
    SHEAR_MODULUS = addDouble(material.G);
}
