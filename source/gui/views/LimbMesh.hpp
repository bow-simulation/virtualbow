#pragma once
#include "solver/model/input/InputData.hpp"
#include "solver/model/ContinuousLimb.hpp"
#include "solver/numerics/Eigen.hpp"
#include <qopengl.h>
#include <QVector3D>
#include <QColor>

#include "OpenGLUtils.hpp"

class LimbMesh
{
public:
    Mesh faces_right;
    Mesh faces_left;

    LimbMesh(const InputData& input);

private:
    void addQuad(QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3, const QColor& color);
    void addLine(QVector3D p0, QVector3D p1, const QColor& color);

    std::vector<double> getEvalLengths(const ContinuousLimb& limb, unsigned n);
};
