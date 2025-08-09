#include "OpenGLUtils.hpp"
#include <QOpenGLShaderProgram>
#include <QColor>

Bounds::Bounds()
    : vec_min(max_inf()), vec_max(min_inf())
{

}

void Bounds::extend(const Bounds& other) {
    vec_min = min(vec_min, other.vec_min);
    vec_max = max(vec_max, other.vec_max);
}

void Bounds::extend(const QVector3D& point) {
    vec_min = min(vec_min, point);
    vec_max = max(vec_max, point);
}

QVector3D Bounds::center() const {
    return (vec_min + vec_max)/2.0f;
}

float Bounds::diagonal() const {
    return (vec_max - vec_min).length();
}

QVector3D Bounds::min(const QVector3D& vec1, const QVector3D& vec2) const {
    return {
        std::min(vec1.x(), vec2.x()),
                std::min(vec1.y(), vec2.y()),
                std::min(vec1.z(), vec2.z())
    };
}

QVector3D Bounds::max(const QVector3D& vec1, const QVector3D& vec2) const {
    return {
        std::max(vec1.x(), vec2.x()),
                std::max(vec1.y(), vec2.y()),
                std::max(vec1.z(), vec2.z())
    };
}

QVector3D Bounds::min_inf() const {
    return {
        -std::numeric_limits<float>::infinity(),
                -std::numeric_limits<float>::infinity(),
                -std::numeric_limits<float>::infinity()
    };
}

QVector3D Bounds::max_inf() const {
    return {
        std::numeric_limits<float>::infinity(),
                std::numeric_limits<float>::infinity(),
                std::numeric_limits<float>::infinity()
    };
}

Mesh::Mesh(unsigned mode)
    : mode(mode) {

}

void Mesh::addVertex(const QVector3D& position, const QVector3D& normal, const QColor& color) {
    vertices.push_back({
        .position = position,
        .normal = normal,
        .color = QVector3D(color.redF(), color.greenF(), color.blueF())
    });
}

void Mesh::addLine(QVector3D p0, QVector3D p1, const QColor& color){
    addVertex(p0, QVector3D(), color);
    addVertex(p1, QVector3D(), color);
}

void Mesh::addTriangle(QVector3D p0, QVector3D p1, QVector3D p2, const QColor& color){
    // Don't use QVector3D::normal() because the implementation arbitrarily decides
    // that a vector with length <= 0.00001 is zero and therefore returns a zero vector.
    // The shader code normalizes the results anyway.
    addVertex(p0, QVector3D::crossProduct(p1 - p0, p2 - p0), color);
    addVertex(p1, QVector3D::crossProduct(p2 - p1, p0 - p1), color);
    addVertex(p2, QVector3D::crossProduct(p0 - p2, p1 - p2), color);
}

void Mesh::addQuad(QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3, const QColor& color){
    // Don't use QVector3D::normal() because the implementation arbitrarily decides
    // that a vector with length <= 0.00001 is zero and therefore returns a zero vector.
    // The shader code normalizes the results anyway.
    addVertex(p0, QVector3D::crossProduct(p1 - p0, p3 - p0), color);
    addVertex(p1, QVector3D::crossProduct(p2 - p1, p0 - p1), color);
    addVertex(p2, QVector3D::crossProduct(p3 - p2, p1 - p2), color);
    addVertex(p3, QVector3D::crossProduct(p0 - p3, p2 - p3), color);
}

const std::vector<Vertex>& Mesh::getVertices() const {
    return vertices;
}

unsigned Mesh::getMode() const {
    return mode;
}

Model::Model(const Mesh& mesh)
    : mesh(mesh), VAO(0), VBO(0)
{

}

Bounds Model::getBounds() const {
    Bounds bounds;
    for(auto& vertex: mesh.getVertices()) {
        bounds.extend(vertex.position);
    }
    return bounds;
}

void Model::draw(QOpenGLShaderProgram* shader) {
    if(VAO == 0) {
        initialize();
    }

    shader->bind();
    glBindVertexArray(VAO);
    glDrawArrays(mesh.getMode(), 0, mesh.getVertices().size());
    glBindVertexArray(0);
    shader->release();
}

void Model::initialize() {
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.getVertices().size()*sizeof(Vertex), mesh.getVertices().data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
