#pragma once
#include "solver/model//input/InputData.hpp"
#include "LimbMesh.hpp"
#include "LayerLegend.hpp"
#include <QtWidgets>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>

// This widget was based on Qt's "Hello GL2" example: http://doc.qt.io/qt-5/qtopengl-hellogl2-example.html
// Uses the Phong lighting model as described here: https://learnopengl.com/Lighting/Basic-Lighting

class LimbView: public QOpenGLWidget, protected QOpenGLFunctions
{
private:
    const QColor CLEAR_COLOR = QColor::fromRgbF(0.2f, 0.3f, 0.4f);

    const QVector3D LIGHT_POSITION = {0.0f, 5.0f, 10.0f};
    const QVector3D CAMERA_POSITION = {0.0f, 0.0f, -1.5f};

    const float MATERIAL_AMBIENT = 0.2f;
    const float MATERIAL_DIFFUSE = 0.8f;
    const float MATERIAL_SPECULAR = 0.1f;
    const float MATERIAL_SHININESS = 8.0f;

    const float DEFAULT_ROT_X = 31.0f;   // Trimetric view
    const float DEFAULT_ROT_Y = -28.0f;  // Trimetric view
    const float DEFAULT_ZOOM = 1.05f;
    const float ZOOM_SPEED = 0.2f;
    const float ROT_SPEED = 0.15f;

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
    int loc_cameraPosition;
    int loc_materialAmbient;
    int loc_materialDiffuse;
    int loc_materialSpecular;
    int loc_materialShininess;

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
