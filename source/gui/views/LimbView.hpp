#pragma once
#include "solver/model/input/InputData.hpp"
#include "LimbMesh.hpp"
#include "LayerLegend.hpp"
#include "OpenGLUtils.hpp"
#include <QtWidgets>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>

class LimbView: public QOpenGLWidget, protected QOpenGLExtraFunctions
{
private:
    //const QColor BACKGROUND_COLOR_1 = QColor::fromRgbF(0.2f, 0.3f, 0.4f).darker(200);
    //const QColor BACKGROUND_COLOR_2 = QColor::fromRgbF(0.2f, 0.3f, 0.4f).lighter(125);

    //const QColor BACKGROUND_COLOR_1 = QColor::fromRgb(100, 100, 100).darker(150);
    //const QColor BACKGROUND_COLOR_2 = QColor::fromRgb(100, 100, 100).lighter(150);

    const QColor BACKGROUND_COLOR_1 = QColor::fromHsv(0, 0, 100 - 50);
    const QColor BACKGROUND_COLOR_2 = QColor::fromHsv(0, 0, 100 + 50);

    const QVector3D CAMERA_POSITION = { 0.0f, 0.0f, 10.0f };
    const QVector3D LIGHT_POSITION = { 0.0f, 0.0f, 10.0f };
    const QVector3D LIGHT_COLOR = { 1.0, 1.0, 1.0 };

    const float MATERIAL_AMBIENT_STRENGTH = 0.2f;
    const float MATERIAL_DIFFUSE_STRENGTH = 0.9f;
    const float MATERIAL_SPECULAR_STRENGTH = 0.5f;
    const float MATERIAL_SHININESS = 32.0f;

    const float DEFAULT_ROT_X = 31.0f;   // Trimetric view
    const float DEFAULT_ROT_Y = -28.0f;  // Trimetric view
    const float DEFAULT_ZOOM = 1.05f;
    const float ZOOM_SPEED = 0.2f;
    const float ROT_SPEED = 0.15f;

public:
    LimbView();
    void setData(const InputData& data);

    void viewProfile();
    void viewTop();
    void view3D();
    void viewSymmetric(bool checked);
    void viewFit();

private:
    void initializeGL() override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;

    LayerLegend* legend;
    LimbMesh limb_mesh_left;
    LimbMesh limb_mesh_right;

    QPoint mouse_pos;
    float shift_x;
    float shift_y;
    float rot_x;
    float rot_y;
    float zoom;

    // OpenGL
    QOpenGLShaderProgram* background_shader;
    QOpenGLShaderProgram* model_shader;
    QOpenGLShaderProgram* edge_shader;
    std::vector<Model> scene_objects;
};
