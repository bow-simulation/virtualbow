#pragma once
#include "bow/input/InputData.hpp"
#include "LimbMesh.hpp"

#include <QtWidgets>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class LimbView: public QOpenGLWidget, protected QOpenGLFunctions
{
private:
    const float ZOOM_SPEED = 0.2f;    // Magic number
    const float ROT_SPEED = 0.15f;    // Magic number

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

    QPoint last_mouse_pos;

    float rot_x;
    float rot_y;
    float shift_x;
    float shift_y;
    float zoom;

    LimbMesh m_mesh;
    QOpenGLBuffer m_meshVbo;

    QOpenGLShaderProgram* m_program;
    int m_projectionMatrixLoc;
    int m_modelViewMatrixLoc;
    int m_normalMatrixLoc;
    int m_lightPositionLoc;

    QMatrix4x4 m_projection;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_world;
};
