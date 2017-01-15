#pragma once
#include "../Plot.hpp"
#include "../../model/Document.hpp"

#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DInput>
#include <Qt3DExtras>

class InputData;

class GeometryView: public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    GeometryView(InputData& data)
        : data(data)
    {

    }

protected:
    void initializeGL()
    {
        // Set up the rendering context, load shaders and other resources, etc.:
        initializeOpenGLFunctions();
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        //...
    }

    void resizeGL(int w, int h)
    {
        // Update projection matrix and other size related settings:
        // m_projection.setToIdentity();
        // m_projection.perspective(45.0f, w / float(h), 0.01f, 100.0f);
        //...
    }

    void paintGL()
    {
        // Draw the scene:
        glClear(GL_COLOR_BUFFER_BIT);
        //...
    }

private:
    InputData& data;
};

/*
class GeometryView: public Qt3DExtras::Qt3DWindow
{
public:
    GeometryView(InputData& data)
        : data(data)
    {
        Qt3DCore::QEntity *scene = createScene();

        // Camera
        Qt3DRender::QCamera *camera = this->camera();
        camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
        camera->setPosition(QVector3D(0, 0, 40.0f));
        camera->setViewCenter(QVector3D(0, 0, 0));

        // For camera controls
        // Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(scene);
        // camController->setLinearSpeed( 50.0f );
        // camController->setLookSpeed( 180.0f );
        // camController->setCamera(camera);

        this->setRootEntity(scene);
    }

private:
    InputData& data;

    Qt3DCore::QEntity *createScene()
    {
        // Root entity
        Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity;

        // Material
        Qt3DRender::QMaterial *material = new Qt3DExtras::QPhongMaterial(rootEntity);

        // Torus
        Qt3DCore::QEntity *torusEntity = new Qt3DCore::QEntity(rootEntity);
        Qt3DExtras::QTorusMesh *torusMesh = new Qt3DExtras::QTorusMesh;
        torusMesh->setRadius(5);
        torusMesh->setMinorRadius(1);
        torusMesh->setRings(100);
        torusMesh->setSlices(20);

        Qt3DCore::QTransform *torusTransform = new Qt3DCore::QTransform;
        torusTransform->setScale3D(QVector3D(1.5, 1, 0.5));
        torusTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 45.0f));

        torusEntity->addComponent(torusMesh);
        torusEntity->addComponent(torusTransform);
        torusEntity->addComponent(material);

        return rootEntity;
    }
};

*/
