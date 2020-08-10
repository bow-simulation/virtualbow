#pragma once
#include "solver/model/input/InputData.hpp"
#include "LimbMesh.hpp"
#include "LayerLegend.hpp"
#include <QtWidgets>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>

struct Vertex {
    QVector3D position;
    QVector3D normal;
    QVector3D color;
};

class BackgroundMesh: protected QOpenGLExtraFunctions
{
public:
    BackgroundMesh(const QColor& color1, const QColor& color2) {
        unsigned i0 = addVertex({ 1.0f, -1.0f, 0.0f}, { 0.0f, 0.0f, 1.0f }, color1);
        unsigned i1 = addVertex({-1.0f, -1.0f, 0.0f}, { 0.0f, 0.0f, 1.0f }, color1);
        unsigned i2 = addVertex({-1.0f,  1.0f, 0.0f}, { 0.0f, 0.0f, 1.0f }, color2);
        unsigned i3 = addVertex({ 1.0f,  1.0f, 0.0f}, { 0.0f, 0.0f, 1.0f }, color2);

        addTriangle(i0, i1, i2);
        addTriangle(i2, i3, i0);

        initializeOpenGLFunctions();

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, color));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void draw(QOpenGLShaderProgram* shader) {
        shader->bind();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        shader->release();
    }

    /*
    const std::vector<Vertex>& getVertices() const {
        return vertices;
    }

    const std::vector<unsigned>& getIndices() const {
        return indices;
    }
    */

private:
    // Add triangle between three indices
    void addTriangle(unsigned i0, unsigned i1, unsigned i2) {
        indices.push_back(i0);
        indices.push_back(i1);
        indices.push_back(i2);
    }

    // Add vertex and return index
    unsigned addVertex(const QVector3D& position, const QVector3D& normal, const QColor& color) {
        vertices.push_back({
            .position = position,
            .normal = normal,
            .color = QVector3D(color.redF(), color.greenF(), color.blueF())
        });

        return vertices.size() - 1;
    }

    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
    unsigned VAO, VBO, EBO;
};

// This widget was based on Qt's "Hello GL2" example: http://doc.qt.io/qt-5/qtopengl-hellogl2-example.html
// Uses the Phong lighting model as described here: https://learnopengl.com/Lighting/Basic-Lighting

class LimbView: public QOpenGLWidget, protected QOpenGLExtraFunctions
{
private:
    const QColor BACKGROUND_COLOR_1 = QColor::fromRgbF(0.2f, 0.3f, 0.4f);
    const QColor BACKGROUND_COLOR_2 = QColor::fromRgbF(0.5f, 0.3f, 0.4f);

    const QVector3D LIGHT_POSITION = { 0.0f, 5.0f, 10.0f };
    const QVector3D CAMERA_POSITION = { 0.0f, 0.0f, -1.5f };

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
    QOpenGLShaderProgram* shader;
    std::unique_ptr<BackgroundMesh> background;
};
