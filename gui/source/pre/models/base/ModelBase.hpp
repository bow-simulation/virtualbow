#pragma once
#include <QObject>

// Base class for all view models

class ModelBase: public QObject {
    Q_OBJECT

public:
    ModelBase();

signals:
    // Emitted after any content of the model, including its geometry, has been modified
    void contentChanged();

    // Emitted only after content of the model that affects its geometry has been modified
    void geometryChanged();

protected slots:
    // Called by implementations when the model has been changed without affecting the bow's displayed geometry
    void nonGeometryModified();

    // Called by implementations when the model has been changed in a way that affects the bow's displayed geometry
    void geometryModified();
};
