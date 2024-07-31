#pragma once
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QVector3D>
#include <memory>

class ViewModel;
class InputData;
class MaterialLegend;
class QOpenGLShaderProgram;
class Model;

class LimbView: public QOpenGLWidget, protected QOpenGLExtraFunctions
{
private:
    const QColor BACKGROUND_COLOR_1 = QColor::fromHsv(200, 30, 120);
    const QColor BACKGROUND_COLOR_2 = QColor::fromHsv(200, 30, 220);

    const QVector3D CAMERA_POSITION = { 0.0f, 0.0f, 10.0f };
    const QVector3D LIGHT_POSITION = { 0.0f, 5.0f, 10.0f };
    const QVector3D LIGHT_COLOR = { 1.0f, 1.0f, 1.0f };

    const float CAMERA_NEAR_PLANE = 0.01f;
    const float CAMERA_FAR_PLANE = 100.0f;

    const float MATERIAL_AMBIENT_STRENGTH = 0.3f;
    const float MATERIAL_DIFFUSE_STRENGTH = 0.8f;
    const float MATERIAL_SPECULAR_STRENGTH = 0.2f;
    const float MATERIAL_SHININESS = 32.0f;

    const float DEFAULT_ROT_X = 31.0f;   // Trimetric view
    const float DEFAULT_ROT_Y = -28.0f;  // Trimetric view
    const float DEFAULT_ZOOM = 1.05f;
    const float ZOOM_SPEED = 0.2f;
    const float ROT_SPEED = 0.15f;

public:
    LimbView(ViewModel* model);
    void updateView();

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

    ViewModel* model;
    MaterialLegend* legend;

    QPoint mouse_pos;
    float shift_x;
    float shift_y;
    float rot_x;
    float rot_y;
    float zoom;
    bool symmetry;

    // OpenGL
    QOpenGLShaderProgram* background_shader;
    QOpenGLShaderProgram* model_shader;

    std::unique_ptr<Model> background;
    std::unique_ptr<Model> limb_right;
    std::unique_ptr<Model> limb_left;
};
