#pragma once
#include "bow/input/InputData.hpp"
#include "numerics/Eigen.hpp"
#include <qopengl.h>
#include <QVector3D>
#include <QColor>

class LimbMesh
{
public:
    LimbMesh() = default;
    void setData(const InputData& data);

    const std::vector<GLfloat>& vertexData() const;
    size_t vertexCount() const;

    QVector3D aabbCenter() const;
    float aabbDiagonal() const;

private:
    void addQuad(const QVector3D& p0, const QVector3D& p1, const QVector3D& p2, const QVector3D& p3, const QColor& color);
    void addVertex(const QVector3D& position, const QVector3D& normal, const QColor& color);
    std::vector<double> getEvalLengths(const InputData& data, unsigned n);

    std::vector<GLfloat> vertex_data;
    QVector3D aabb_min;
    QVector3D aabb_max;
};
