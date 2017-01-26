#pragma once
#include "../Plot.hpp"
#include "../../model/Document.hpp"

// OpenGL approach...
/*
class InputData;

class GeometryView: public QOpenGLWidget, protected QOpenGLExtraFunctions
{
public:
    GeometryView(InputData& data)
        : data(data),
          rot_x(0),
          rot_y(0),
          rot_z(0)
    {

    }

    virtual QSize minimumSizeHint() const override
    {
        return QSize(100, 50);
    }

    virtual QSize sizeHint() const override
    {
        return QSize(400, 200);
    }

protected:
    virtual void initializeGL() override
    {
        initializeOpenGLFunctions();
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    }

    virtual void resizeGL(int w, int h) override
    {

    }

    virtual void paintGL() override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw();
    }

private:
    InputData& data;

    int rot_x;
    int rot_y;
    int rot_z;

    void draw() const
    {
        glBegin(GL_TRIANGLES);
            glVertex3f( 0.0f, 1.0f, 0.0f);
            glVertex3f(-1.0f,-1.0f, 0.0f);
            glVertex3f( 1.0f,-1.0f, 0.0f);
        glEnd();
    }

};
*/



// Qt3D approach...

#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DInput>
#include <Qt3DExtras>

class InputData;

class GeometryView: public Qt3DExtras::Qt3DWindow
{
public:
    GeometryView(InputData& data)
        : data(data)
    {
        auto *scene = createScene();

        // Camera
        auto *camera = this->camera();
        camera->setPosition(QVector3D(0.0f, 0.0f, 10.0f));
        camera->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));
        camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);

        // For camera controls
        auto *camController = new Qt3DExtras::QFirstPersonCameraController(scene);  //new Qt3DExtras::QOrbitCameraController(scene);
        camController->setLinearSpeed( 50.0f );
        camController->setLookSpeed( 180.0f );
        camController->setCamera(camera);

        // Light
        Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(scene);
        Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(lightEntity);
        light->setColor("white");
        light->setIntensity(1);
        lightEntity->addComponent(light);
        Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
        lightTransform->setTranslation(camera->position());
        lightEntity->addComponent(lightTransform);

        this->defaultFrameGraph()->setClearColor(0x4d4d4f);
        this->setRootEntity(scene);
    }

private:
    InputData& data;

    Qt3DCore::QEntity *createScene()
    {
        // Root entity
        auto rootEntity = new Qt3DCore::QEntity;

        // Mesh entity
        Qt3DCore::QEntity *customMeshEntity = new Qt3DCore::QEntity(rootEntity);

        // Material
        //Qt3DRender::QMaterial *material = new Qt3DExtras::QPerVertexColorMaterial(rootEntity);

        Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial(rootEntity);
        material->setDiffuse("blue");

        //Qt3DExtras::QGoochMaterial *material = new Qt3DExtras::QGoochMaterial(rootEntity);


        // Transform
        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
        transform->setScale(8.0f);

        // Custom Mesh (TetraHedron)
        Qt3DRender::QGeometryRenderer *customMeshRenderer = new Qt3DRender::QGeometryRenderer;
        Qt3DRender::QGeometry *customGeometry = new Qt3DRender::QGeometry(customMeshRenderer);

        Qt3DRender::QBuffer *vertexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, customGeometry);
        Qt3DRender::QBuffer *indexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, customGeometry);

        // vec3 for position
        // vec3 for colors
        // vec3 for normals

        /*          2
                   /|\
                  / | \
                 / /3\ \
                 0/___\ 1
        */

        // 4 distinct vertices
        QByteArray vertexBufferData;
        vertexBufferData.resize(4 * (3 + 3 + 3) * sizeof(float));

        // Vertices
        QVector3D v0(-1.0f, 0.0f, -1.0f);
        QVector3D v1( 1.0f, 0.0f, -1.0f);
        QVector3D v2( 0.0f, 1.0f,  0.0f);
        QVector3D v3( 0.0f, 0.0f,  1.0f);

        // Faces Normals
        QVector3D n023 = QVector3D::normal(v0, v2, v3);
        QVector3D n012 = QVector3D::normal(v0, v1, v2);
        QVector3D n310 = QVector3D::normal(v3, v1, v0);
        QVector3D n132 = QVector3D::normal(v1, v3, v2);

        // Vector Normals
        QVector3D n0 = QVector3D(n023 + n012 + n310).normalized();
        QVector3D n1 = QVector3D(n132 + n012 + n310).normalized();
        QVector3D n2 = QVector3D(n132 + n012 + n023).normalized();
        QVector3D n3 = QVector3D(n132 + n310 + n023).normalized();

        // Color
        QVector<QVector3D> vertices = QVector<QVector3D>()
                << v0 << n0
                << v1 << n1
                << v2 << n2
                << v3 << n3;

        float *rawVertexArray = reinterpret_cast<float*>(vertexBufferData.data());
        int idx = 0;

        Q_FOREACH(const QVector3D &v, vertices)
        {
            rawVertexArray[idx++] = v.x();
            rawVertexArray[idx++] = v.y();
            rawVertexArray[idx++] = v.z();
        }

        // Indices (12)
        QByteArray indexBufferData;
        indexBufferData.resize(4 * 3 * sizeof(ushort));
        ushort *rawIndexArray = reinterpret_cast<ushort *>(indexBufferData.data());

        // Front
        rawIndexArray[0] = 0;
        rawIndexArray[1] = 1;
        rawIndexArray[2] = 2;
        // Bottom
        rawIndexArray[3] = 3;
        rawIndexArray[4] = 1;
        rawIndexArray[5] = 0;
        // Left
        rawIndexArray[6] = 0;
        rawIndexArray[7] = 2;
        rawIndexArray[8] = 3;
        // Right
        rawIndexArray[9] = 1;
        rawIndexArray[10] = 3;
        rawIndexArray[11] = 2;

        vertexDataBuffer->setData(vertexBufferData);
        indexDataBuffer->setData(indexBufferData);

        // Attributes
        Qt3DRender::QAttribute *positionAttribute = new Qt3DRender::QAttribute();
        positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        positionAttribute->setBuffer(vertexDataBuffer);
        positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
        positionAttribute->setVertexSize(3);
        positionAttribute->setByteOffset(0);
        positionAttribute->setByteStride(6 * sizeof(float));
        positionAttribute->setCount(4);
        positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

        Qt3DRender::QAttribute *normalAttribute = new Qt3DRender::QAttribute();
        normalAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        normalAttribute->setBuffer(vertexDataBuffer);
        normalAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
        normalAttribute->setVertexSize(3);
        normalAttribute->setByteOffset(3 * sizeof(float));
        normalAttribute->setByteStride(6 * sizeof(float));
        normalAttribute->setCount(4);
        normalAttribute->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());

        Qt3DRender::QAttribute *indexAttribute = new Qt3DRender::QAttribute();
        indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
        indexAttribute->setBuffer(indexDataBuffer);
        indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedShort);
        indexAttribute->setVertexSize(1);
        indexAttribute->setByteOffset(0);
        indexAttribute->setByteStride(0);
        indexAttribute->setCount(12);

        customGeometry->addAttribute(positionAttribute);
        customGeometry->addAttribute(normalAttribute);
        customGeometry->addAttribute(indexAttribute);

        customMeshRenderer->setInstanceCount(1);
        customMeshRenderer->setIndexOffset(0);
        customMeshRenderer->setFirstInstance(0);
        customMeshRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
        customMeshRenderer->setGeometry(customGeometry);
        // 4 faces of 3 points
        customMeshRenderer->setVertexCount(12);

        customMeshEntity->addComponent(customMeshRenderer);
        customMeshEntity->addComponent(transform);
        customMeshEntity->addComponent(material);

        return rootEntity;
    }

/*
    Qt3DCore::QEntity *createScene()
    {
        // Root entity
        auto rootEntity = new Qt3DCore::QEntity;

        // Mesh entity
        Qt3DCore::QEntity *customMeshEntity = new Qt3DCore::QEntity(rootEntity);

        // Material
        //Qt3DRender::QMaterial *material = new Qt3DExtras::QPerVertexColorMaterial(rootEntity);

        Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial(rootEntity);
        material->setDiffuse("blue");

        //Qt3DExtras::QGoochMaterial *material = new Qt3DExtras::QGoochMaterial(rootEntity);


        // Transform
        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
        transform->setScale(8.0f);

        // Custom Mesh (TetraHedron)
        Qt3DRender::QGeometryRenderer *customMeshRenderer = new Qt3DRender::QGeometryRenderer;
        Qt3DRender::QGeometry *customGeometry = new Qt3DRender::QGeometry(customMeshRenderer);

        Qt3DRender::QBuffer *vertexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, customGeometry);
        Qt3DRender::QBuffer *indexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, customGeometry);

        // vec3 for position
        // vec3 for colors
        // vec3 for normals

        // 4 distinct vertices
        QByteArray vertexBufferData;
        vertexBufferData.resize(4 * (3 + 3 + 3) * sizeof(float));

        // Vertices
        QVector3D v0(-1.0f, 0.0f, -1.0f);
        QVector3D v1( 1.0f, 0.0f, -1.0f);
        QVector3D v2( 0.0f, 1.0f,  0.0f);
        QVector3D v3( 0.0f, 0.0f,  1.0f);

        // Faces Normals
        QVector3D n023 = QVector3D::normal(v0, v2, v3);
        QVector3D n012 = QVector3D::normal(v0, v1, v2);
        QVector3D n310 = QVector3D::normal(v3, v1, v0);
        QVector3D n132 = QVector3D::normal(v1, v3, v2);

        // Vector Normals
        QVector3D n0 = QVector3D(n023 + n012 + n310).normalized();
        QVector3D n1 = QVector3D(n132 + n012 + n310).normalized();
        QVector3D n2 = QVector3D(n132 + n012 + n023).normalized();
        QVector3D n3 = QVector3D(n132 + n310 + n023).normalized();

        // Color
        QVector3D color(0.0f, 0.0f, 1.0f);

        QVector<QVector3D> vertices = QVector<QVector3D>()
                << v0 << n0 << color
                << v1 << n1 << color
                << v2 << n2 << color
                << v3 << n3 << color;

        float *rawVertexArray = reinterpret_cast<float*>(vertexBufferData.data());
        int idx = 0;

        Q_FOREACH(const QVector3D &v, vertices)
        {
            rawVertexArray[idx++] = v.x();
            rawVertexArray[idx++] = v.y();
            rawVertexArray[idx++] = v.z();
        }

        // Indices (12)
        QByteArray indexBufferData;
        indexBufferData.resize(4 * 3 * sizeof(ushort));
        ushort *rawIndexArray = reinterpret_cast<ushort *>(indexBufferData.data());

        // Front
        rawIndexArray[0] = 0;
        rawIndexArray[1] = 1;
        rawIndexArray[2] = 2;
        // Bottom
        rawIndexArray[3] = 3;
        rawIndexArray[4] = 1;
        rawIndexArray[5] = 0;
        // Left
        rawIndexArray[6] = 0;
        rawIndexArray[7] = 2;
        rawIndexArray[8] = 3;
        // Right
        rawIndexArray[9] = 1;
        rawIndexArray[10] = 3;
        rawIndexArray[11] = 2;

        vertexDataBuffer->setData(vertexBufferData);
        indexDataBuffer->setData(indexBufferData);

        // Attributes
        Qt3DRender::QAttribute *positionAttribute = new Qt3DRender::QAttribute();
        positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        positionAttribute->setBuffer(vertexDataBuffer);
        positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
        positionAttribute->setVertexSize(3);
        positionAttribute->setByteOffset(0);
        positionAttribute->setByteStride(9 * sizeof(float));
        positionAttribute->setCount(4);
        positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

        Qt3DRender::QAttribute *normalAttribute = new Qt3DRender::QAttribute();
        normalAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        normalAttribute->setBuffer(vertexDataBuffer);
        normalAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
        normalAttribute->setVertexSize(3);
        normalAttribute->setByteOffset(3 * sizeof(float));
        normalAttribute->setByteStride(9 * sizeof(float));
        normalAttribute->setCount(4);
        normalAttribute->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());

        Qt3DRender::QAttribute *indexAttribute = new Qt3DRender::QAttribute();
        indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
        indexAttribute->setBuffer(indexDataBuffer);
        indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedShort);
        indexAttribute->setVertexSize(1);
        indexAttribute->setByteOffset(0);
        indexAttribute->setByteStride(0);
        indexAttribute->setCount(12);

        customGeometry->addAttribute(positionAttribute);
        customGeometry->addAttribute(normalAttribute);
        customGeometry->addAttribute(indexAttribute);

        customMeshRenderer->setInstanceCount(1);
        customMeshRenderer->setIndexOffset(0);
        customMeshRenderer->setFirstInstance(0);
        customMeshRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
        customMeshRenderer->setGeometry(customGeometry);
        // 4 faces of 3 points
        customMeshRenderer->setVertexCount(12);

        customMeshEntity->addComponent(customMeshRenderer);
        customMeshEntity->addComponent(transform);
        customMeshEntity->addComponent(material);

        return rootEntity;
    }
*/

    Qt3DCore::QEntity *createGeometry()
    {

    }

    Qt3DCore::QEntity *createSceneB()
    {
        // Root entity
        auto root = new Qt3DCore::QEntity;

        // Material, mesh, transform
        auto material = new Qt3DExtras::QPhongMaterial(root);

        auto mesh = new Qt3DExtras::QCuboidMesh;
        mesh->setXExtent(1.0f);
        mesh->setYExtent(1.0f);
        mesh->setZExtent(1.0f);

        auto transform = new Qt3DCore::QTransform;
        transform->setScale3D(QVector3D(1.5, 1, 0.5));
        transform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 45.0f));

        // Model
        auto model = new Qt3DCore::QEntity(root);
        model->addComponent(mesh);
        model->addComponent(transform);
        model->addComponent(material);

        return root;
    }
};
