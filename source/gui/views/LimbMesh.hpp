#pragma once
#include "solver/model//input/InputData.hpp"
#include "solver/model//ContinuousLimb.hpp"
#include "solver/numerics/Eigen.hpp"
#include <qopengl.h>
#include <QVector3D>
#include <QColor>

#include "OpenGLUtils.hpp"

struct LimbMeshes {
    Mesh limb_faces_right;
    Mesh limb_edges_right;
    Mesh limb_faces_left;
    Mesh limb_edges_left;

    LimbMeshes(const InputData& input);
};

class LimbMesh
{
public:
    static Mesh createFaces(const InputData& input);
    static Mesh createEdges(const InputData& input);
    static Mesh createFaces2(const InputData& input);

    LimbMesh(bool inverted);
    void setData(const InputData& data);

    const std::vector<GLfloat>& vertexData() const;
    size_t vertexCount() const;
    //const AABB aabb() const;

    bool isVisible() const;
    void setVisible(bool value);

private:
    void addQuad(QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3, const QColor& color);
    void addVertex(const QVector3D& position, const QVector3D& normal, const QColor& color);
    std::vector<double> getEvalLengths(const ContinuousLimb& limb, unsigned n);

    bool visible;
    bool inverted;
    std::vector<GLfloat> vertex_data;
    //AABB bounding_box;
};
