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

private:
    void addQuad(const Vector<3>& p0, const Vector<3>& p1, const Vector<3>& p2, const Vector<3>& p3, const QColor& color);
    void addVertex(const Vector<3>& position, const Vector<3>& normal, const QColor& color);
    std::vector<double> getEvalLengths(const InputData& data, unsigned n);

    std::vector<GLfloat> vertex_data;
};
