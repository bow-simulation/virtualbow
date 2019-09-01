#pragma once
#include "bow/input/InputData.hpp"
#include "bow/ContinuousLimb.hpp"
#include "numerics/Eigen.hpp"
#include "AABB.hpp"
#include <qopengl.h>
#include <QVector3D>
#include <QColor>

class LimbMesh
{
public:
    LimbMesh(bool inverted);
    void setData(const InputData& data);

    const std::vector<GLfloat>& vertexData() const;
    size_t vertexCount() const;
    const AABB aabb() const;

    bool isVisible() const;
    void setVisible(bool value);

private:
    void addQuad(QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3, const QColor& color);
    void addVertex(const QVector3D& position, const QVector3D& normal, const QColor& color);
    std::vector<double> getEvalLengths(const ContinuousLimb& limb, unsigned n);

    bool visible;
    bool inverted;
    std::vector<GLfloat> vertex_data;
    AABB bounding_box;
};
