#pragma once
#include <QOpenGLExtraFunctions>
#include <QOpenGLShader>
#include <QVector3D>
#include <QColor>

struct Vertex
{
    QVector3D position;
    QVector3D normal;
    QVector3D color;
};

class AABB
{
public:
    AABB(): AABB(max_inf(), min_inf()) {

    }

    AABB(const QVector3D& vec_min, const QVector3D& vec_max)
        : vec_min(vec_min), vec_max(vec_max) {

    }

    AABB extend(const AABB& other) const {
        return {
            min(vec_min, other.vec_min),
            max(vec_max, other.vec_max)
        };
    }

    AABB extend(const QVector3D& point) const{
        return {
            min(vec_min, point),
            max(vec_max, point)
        };
    }

    QVector3D center() const {
        return (vec_min + vec_max)/2.0f;
    }

    float diagonal() const {
        return (vec_max - vec_min).length();
    }

private:
    QVector3D vec_min;
    QVector3D vec_max;

    QVector3D min(const QVector3D& vec1, const QVector3D& vec2) const {
        return {
            std::min(vec1.x(), vec2.x()),
            std::min(vec1.y(), vec2.y()),
            std::min(vec1.z(), vec2.z())
        };
    }

    QVector3D max(const QVector3D& vec1, const QVector3D& vec2) const {
        return {
            std::max(vec1.x(), vec2.x()),
            std::max(vec1.y(), vec2.y()),
            std::max(vec1.z(), vec2.z())
        };
    }

    QVector3D min_inf() const {
        return {
            -std::numeric_limits<float>::infinity(),
            -std::numeric_limits<float>::infinity(),
            -std::numeric_limits<float>::infinity()
        };
    }

    QVector3D max_inf() const {
        return {
            std::numeric_limits<float>::infinity(),
            std::numeric_limits<float>::infinity(),
            std::numeric_limits<float>::infinity()
        };
    }
};

class Mesh
{
public:
    // Add vertex and return index
    unsigned addVertex(const QVector3D& position, const QVector3D& normal, const QColor& color) {
        vertices.push_back({
            .position = position,
            .normal = normal,
            .color = QVector3D(color.redF(), color.greenF(), color.blueF())
        });

        return vertices.size() - 1;
    }

    // Add triangle between three indices
    void addTriangle(unsigned i0, unsigned i1, unsigned i2) {
        indices.push_back(i0);
        indices.push_back(i1);
        indices.push_back(i2);
    }

    const std::vector<Vertex>& getVertices() const {
        return vertices;
    }

    const std::vector<unsigned>& getIndices() const {
        return indices;
    }

    AABB getBounds() const {
        AABB aabb;
        for(unsigned index: indices) {
            aabb.extend(vertices[index].position);
        }
        return aabb;
    }

private:
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
};

class Model: protected QOpenGLExtraFunctions
{
public:
    Model(const Mesh& aMesh, QOpenGLShaderProgram* aShader)
        : mesh(aMesh),
          shader(aShader)
    {
        initializeOpenGLFunctions();

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.getVertices().size()*sizeof(Vertex), mesh.getVertices().data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.getIndices().size()*sizeof(unsigned), mesh.getIndices().data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, color));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void draw() {
        shader->bind();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, mesh.getIndices().size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        shader->release();
    }

private:
    Mesh mesh;
    QOpenGLShaderProgram* shader;
    unsigned VAO, VBO, EBO;
};
