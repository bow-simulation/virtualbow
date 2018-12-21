#pragma once
#include "bow/input/InputData.hpp"
#include "numerics/Eigen.hpp"
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

    QVector3D aabbCenter() const;
    float aabbDiagonal() const;

private:
    void addQuad(QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3, const QColor& color);
    void addVertex(const QVector3D& position, const QVector3D& normal, const QColor& color);
    std::vector<double> getEvalLengths(const InputData& data, unsigned n);

    bool inverted;
    std::vector<GLfloat> vertex_data;
    QVector3D aabb_min;
    QVector3D aabb_max;
};
