#include "LimbMesh.hpp"
#include "bow/LimbProperties.hpp"
#include <qmath.h>

void LimbMesh::setData(const InputData& data)
{
    m_data.resize(2500*6);
    m_count = 0;

    LimbProperties limb(data, 150);

    double w = limb.width(0);
    double h = limb.height(0);

    quad(+0.5*w, +0.5*h,
         -0.5*w, +0.5*h,
         -0.5*w, -0.5*h,
         +0.5*w, -0.5*h);
}

const GLfloat* LimbMesh::constData() const
{
    return m_data.constData();
}

int LimbMesh::count() const
{
    return m_count;
}

int LimbMesh::vertexCount() const
{
    return m_count/6;
}

void LimbMesh::quad(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4)
{
    QVector3D n = QVector3D::normal(QVector3D(x4 - x1, y4 - y1, 0.0f), QVector3D(x2 - x1, y2 - y1, 0.0f));

    add(QVector3D(x1, y1, -0.05f), n);
    add(QVector3D(x4, y4, -0.05f), n);
    add(QVector3D(x2, y2, -0.05f), n);

    add(QVector3D(x3, y3, -0.05f), n);
    add(QVector3D(x2, y2, -0.05f), n);
    add(QVector3D(x4, y4, -0.05f), n);
}

void LimbMesh::add(const QVector3D& v, const QVector3D& n)
{
    GLfloat* p = m_data.data() + m_count;
    *p++ = v.x();
    *p++ = v.y();
    *p++ = v.z();
    *p++ = n.x();
    *p++ = n.y();
    *p++ = n.z();
    m_count += 6;
}
