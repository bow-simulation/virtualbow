#pragma once
#include "OpenGLUtils.hpp"
#include <qopengl.h>

class BowModel;
class LimbInfo;
class QVector3D;
class QColor;

class LimbMesh
{
public:
    Mesh faces_right;
    Mesh faces_left;

    LimbMesh(const BowModel& bow, const LimbInfo& geometry);

private:
    void addQuad(QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3, const QColor& color);
};
