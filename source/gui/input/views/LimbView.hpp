#pragma once
#include "bow/input/InputData.hpp"
#include "LimbMesh.hpp"
#include "LayerLegend.hpp"

#include <QtWidgets>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

// Based on Qt's "Hello GL2" example, http://doc.qt.io/qt-5/qtopengl-hellogl2-example.html
class LimbView: public QOpenGLWidget, protected QOpenGLFunctions
{
private:
    const float DEFAULT_ROT_X = 31.0f;   // Trimetric view
    const float DEFAULT_ROT_Y = -28.0f;  // Trimetric view
    const float DEFAULT_ZOOM = 1.1f;     // Magic number
    const float ZOOM_SPEED = 0.2f;       // Magic number
    const float ROT_SPEED = 0.15f;       // Magic number

public:
    LimbView();
    ~LimbView() override;

    void setData(const InputData& data);

    void viewProfile();
    void viewTop();
    void view3D();
    void viewSymmetric(bool checked);
    void viewFit();

private:
    void cleanup();

    void initializeGL() override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;

    LayerLegend* legend;
    LimbMesh limb_mesh_left;
    LimbMesh limb_mesh_right;
    QOpenGLBuffer limb_mesh_left_vbo;
    QOpenGLBuffer limb_mesh_right_vbo;

    QOpenGLShaderProgram* shader_program;
    int loc_projectionMatrix;
    int loc_modelViewMatrix;
    int loc_normalMatrix;
    int loc_lightPosition;

    QMatrix4x4 m_projection;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_world;

    QPoint mouse_pos;
    float shift_x;
    float shift_y;
    float rot_x;
    float rot_y;
    float zoom;
};
