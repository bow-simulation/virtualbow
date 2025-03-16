#pragma once
#include "solver/Input.hpp"
#include <qopengl.h>
#include <QVector3D>
#include <QColor>

#include "OpenGLUtils.hpp"

class LimbMesh
{
public:
    Mesh faces_right;
    Mesh faces_left;

    LimbMesh(const BowModel& input);

private:
    void addQuad(QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3, const QColor& color);
    void addLine(QVector3D p0, QVector3D p1, const QColor& color);

    //std::vector<double> getEvalLengths(const ContinuousLimb& limb, unsigned n);
};
