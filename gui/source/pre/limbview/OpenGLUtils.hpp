#pragma once
#include <QVector3D>
#include <QOpenGLExtraFunctions>

class QOpenGLShaderProgram;

struct Vertex
{
    QVector3D position;
    QVector3D normal;
    QVector3D color;
};

class Bounds {
public:
    Bounds();

    void extend(const Bounds& other);
    void extend(const QVector3D& point);

    QVector3D center() const;
    float diagonal() const;

private:
    QVector3D vec_min;
    QVector3D vec_max;

    QVector3D min(const QVector3D& vec1, const QVector3D& vec2) const;
    QVector3D max(const QVector3D& vec1, const QVector3D& vec2) const;
    QVector3D min_inf() const;
    QVector3D max_inf() const;
};

class Mesh
{
public:
    Mesh(unsigned mode);

    void addVertex(const QVector3D& position, const QVector3D& normal, const QColor& color);
    void addLine(QVector3D p0, QVector3D p1, const QColor& color);
    void addTriangle(QVector3D p0, QVector3D p1, QVector3D p2, const QColor& color);
    void addQuad(QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3, const QColor& color);

    const std::vector<Vertex>& getVertices() const;
    unsigned getMode() const;

private:
    std::vector<Vertex> vertices;
    unsigned mode;
};

class Model: protected QOpenGLExtraFunctions
{
public:
    Model(const Mesh& mesh);

    Bounds getBounds() const;
    void draw(QOpenGLShaderProgram* shader);

private:
    Mesh mesh;
    unsigned VAO;
    unsigned VBO;

    void initialize();
};
