#pragma once
#include "bow/input/InputData.hpp"
#include <qopengl.h>
#include <QVector>
#include <QVector3D>
#include <QColor>

class LimbMesh
{
public:
    LimbMesh() = default;
    void setData(const InputData& data);

    const GLfloat* data() const;
    int count() const;
    int vertexCount() const;

private:
    void addQuad(const QVector3D& p0, const QVector3D& p1, const QVector3D& p2, const QVector3D& p3, const QColor& color);
    void addVertex(const QVector3D& position, const QVector3D& normal, const QColor& color);

    std::vector<double> getEvalLengths(const InputData& data, unsigned n);

    QVector<GLfloat> m_data;
}; 
