#include "ModelBase.hpp"

ModelBase::ModelBase() {
    // Content changed is emitted on any modifications
    QObject::connect(this, &ModelBase::nonGeometryModified, this, &ModelBase::contentChanged);
    QObject::connect(this, &ModelBase::geometryModified, this, &ModelBase::contentChanged);

    // Geometry changed is emitted only on geometry modifications
    QObject::connect(this, &ModelBase::geometryModified, this, &ModelBase::geometryChanged);
}

void ModelBase::nonGeometryModified() {

}

void ModelBase::geometryModified() {

}
