#pragma once
#include "bow/input/InputData.hpp"
#include <qopengl.h>
#include <QVector>
#include <QVector3D>

class LimbMesh
{
public:
    LimbMesh() = default;
    void setData(const InputData& data);

    const GLfloat* constData() const;
    int count() const;
    int vertexCount() const;

private:
    void quad(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4);
    void add(const QVector3D& v, const QVector3D& n);

    QVector<GLfloat> m_data;
    int m_count;
}; 
