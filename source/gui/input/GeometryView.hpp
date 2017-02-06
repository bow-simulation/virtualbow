#pragma once
#include "../Plot.hpp"
#include "../../model/Document.hpp"

#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DInput>
#include <Qt3DExtras>

#include "../../model/DiscreteLimb.hpp"

class ModelEntity: public Qt3DCore::QEntity
{
public:
    ModelEntity(QNode* parent = nullptr)
        : Qt3DCore::QEntity(parent)
    {
        // Geometry

        vertices = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, this);
        normals = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, this);
        indices = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, this);

        Qt3DRender::QAttribute *positionAttribute = new Qt3DRender::QAttribute();
        positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
        positionAttribute->setBuffer(vertices);
        positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
        positionAttribute->setVertexSize(3);
        positionAttribute->setByteOffset(0);
        positionAttribute->setByteStride(3*sizeof(float));
        positionAttribute->setCount(n_vertices);

        Qt3DRender::QAttribute *normalAttribute = new Qt3DRender::QAttribute();
        normalAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        normalAttribute->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());
        normalAttribute->setBuffer(normals);
        normalAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
        normalAttribute->setVertexSize(3);
        normalAttribute->setByteOffset(0);
        normalAttribute->setByteStride(3*sizeof(float));
        normalAttribute->setCount(n_vertices);

        Qt3DRender::QAttribute *indexAttribute = new Qt3DRender::QAttribute();
        indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
        indexAttribute->setBuffer(indices);
        indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedShort);
        indexAttribute->setVertexSize(1);
        indexAttribute->setByteOffset(0);
        indexAttribute->setByteStride(0);
        indexAttribute->setCount(n_indices);

        Qt3DRender::QGeometry* geometry = new Qt3DRender::QGeometry;
        geometry->addAttribute(positionAttribute);
        geometry->addAttribute(normalAttribute);
        geometry->addAttribute(indexAttribute);

        // Mesh renderer

        Qt3DRender::QGeometryRenderer* renderer = new Qt3DRender::QGeometryRenderer;
        renderer->setGeometry(geometry);
        //renderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Points);

        //renderer->setInstanceCount(1);
        //renderer->setIndexOffset(0);
        //renderer->setFirstInstance(0);
        //renderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
        //renderer->setVertexCount(12);

        // Material

        Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial;
        material->setDiffuse(0x000066); // Magic number

        // Transform

        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;

        this->addComponent(renderer);
        this->addComponent(material);
        this->addComponent(transform);
    }

    void update(const InputData& input)
    {
        this->setEnabled(true);

        try
        {
            // Interpolate and sample geometric data
            Curve profile = ArcCurve::sample(input.profile_segments, input.profile_x0, input.profile_y0, input.profile_phi0, n_segments);
            Series width = CubicSpline::sample(input.sections_width, n_segments);
            Series height = CubicSpline::sample(input.sections_height, n_segments);

            // Update vertex and index buffers
            setGeometryData(profile, width, height);
        }
        catch(const std::runtime_error&)  // Todo
        {
            this->setEnabled(false);
        }
    }

    void setGeometryData(const Curve& profile, const Series& width, const Series& height)
    {
        // Vertex, normal and index arrays

        QByteArray vertex_array;
        vertex_array.resize(3*n_vertices*sizeof(float));
        float* vertex_data = reinterpret_cast<float*>(vertex_array.data());

        QByteArray normal_array;
        normal_array.resize(3*n_vertices*sizeof(float));
        float* normal_data = reinterpret_cast<float*>(normal_array.data());

        QByteArray index_array;
        index_array.resize(n_indices*sizeof(ushort));
        ushort* index_data = reinterpret_cast<ushort*>(index_array.data());

        // Calculate vertices and normals

        size_t iv = 0;
        size_t in = 0;
        for(size_t i = 0; i <= n_segments; ++i)
        {
            // Curve point, tangent and normal

            QVector3D p(profile.x[i], profile.y[i], 0.0f);
            QVector3D t(std::cos(profile.phi[i]), std::sin(profile.phi[i]), 0.0f);
            QVector3D n(-std::sin(profile.phi[i]), std::cos(profile.phi[i]), 0.0f);

            // Cross section vertices

            QVector3D p0 = p - QVector3D(0.0f, 0.0f, 0.5*width.val(i));
            QVector3D p1 = p + QVector3D(0.0f, 0.0f, 0.5*width.val(i));
            QVector3D p2 = p0 + height.val(i)*n;
            QVector3D p3 = p1 + height.val(i)*n;

            // Surface normals

            QVector3D n0 = (QVector3D(0.0f, 0.0f, -1.0f) - n).normalized();
            QVector3D n1 = (QVector3D(0.0f, 0.0f,  1.0f) - n).normalized();
            QVector3D n2 = (QVector3D(0.0f, 0.0f, -1.0f) + n).normalized();
            QVector3D n3 = (QVector3D(0.0f, 0.0f,  1.0f) + n).normalized();

            if(i == 0)
            {
                n0 = (QVector3D(0.0f, 0.0f, -1.0f) - n - t).normalized();
                n1 = (QVector3D(0.0f, 0.0f,  1.0f) - n - t).normalized();
                n2 = (QVector3D(0.0f, 0.0f, -1.0f) + n - t).normalized();
                n3 = (QVector3D(0.0f, 0.0f,  1.0f) + n - t).normalized();
            }
            else if(i == n_segments)
            {
                n0 = (QVector3D(0.0f, 0.0f, -1.0f) - n + t).normalized();
                n1 = (QVector3D(0.0f, 0.0f,  1.0f) - n + t).normalized();
                n2 = (QVector3D(0.0f, 0.0f, -1.0f) + n + t).normalized();
                n3 = (QVector3D(0.0f, 0.0f,  1.0f) + n + t).normalized();
            }

            // Write data to array    // Todo: More elegant solution?

            vertex_data[iv++] = p0.x(); vertex_data[iv++] = p0.y(); vertex_data[iv++] = p0.z();
            vertex_data[iv++] = p1.x(); vertex_data[iv++] = p1.y(); vertex_data[iv++] = p1.z();
            vertex_data[iv++] = p2.x(); vertex_data[iv++] = p2.y(); vertex_data[iv++] = p2.z();
            vertex_data[iv++] = p3.x(); vertex_data[iv++] = p3.y(); vertex_data[iv++] = p3.z();

            normal_data[in++] = n0.x(); normal_data[in++] = n0.y(); normal_data[in++] = n0.z();
            normal_data[in++] = n1.x(); normal_data[in++] = n1.y(); normal_data[in++] = n1.z();
            normal_data[in++] = n2.x(); normal_data[in++] = n2.y(); normal_data[in++] = n2.z();
            normal_data[in++] = n3.x(); normal_data[in++] = n3.y(); normal_data[in++] = n3.z();
        }

        // Calculate indices

        qInfo() << "-----------";

        size_t ii = 0;
        for(size_t i = 0; i < n_segments; ++i)
        {
            size_t i0 = 4*i;        // Beginning index of first section
            size_t i1 = i0 + 4;     // Beginning index of second section

            if(i == 0)
            {
                index_data[ii++] = i0 + 0; index_data[ii++] = i0 + 1; index_data[ii++] = i0 + 2;
                index_data[ii++] = i0 + 1; index_data[ii++] = i0 + 3; index_data[ii++] = i0 + 2;
            }
            else if(i == n_segments - 1)
            {
                index_data[ii++] = i1 + 2; index_data[ii++] = i1 + 1; index_data[ii++] = i1 + 0;
                index_data[ii++] = i1 + 2; index_data[ii++] = i1 + 3; index_data[ii++] = i1 + 1;
            }

            index_data[ii++] = i0 + 3; index_data[ii++] = i0 + 1; index_data[ii++] = i1 + 1;
            index_data[ii++] = i1 + 1; index_data[ii++] = i1 + 3; index_data[ii++] = i0 + 3;

            index_data[ii++] = i0 + 0; index_data[ii++] = i0 + 2; index_data[ii++] = i1 + 0;
            index_data[ii++] = i1 + 2; index_data[ii++] = i1 + 0; index_data[ii++] = i0 + 2;

            index_data[ii++] = i0 + 2; index_data[ii++] = i0 + 3; index_data[ii++] = i1 + 3;
            index_data[ii++] = i1 + 3; index_data[ii++] = i1 + 2; index_data[ii++] = i0 + 2;

            index_data[ii++] = i0 + 1; index_data[ii++] = i0 + 0; index_data[ii++] = i1 + 1;
            index_data[ii++] = i1 + 0; index_data[ii++] = i1 + 1; index_data[ii++] = i0 + 0;
        }

/*
        index_data[0] = 0;
        index_data[1] = 1;
        index_data[2] = 2;

        index_data[3] = 1;
        index_data[4] = 3;
        index_data[5] = 2;
*/

        // Assign data to buffers

        vertices->setData(vertex_array);
        normals->setData(normal_array);
        indices->setData(index_array);
    }

    /*
    void setGeometryData(const Curve& profile, const Series& width, const Series& height)
    {
        // Vertices

        QByteArray vertex_array;
        vertex_array.resize(3*n_vertices*sizeof(float));
        float* vertex_data = reinterpret_cast<float*>(vertex_array.data());

        vertex_data[0] = 0.0f;
        vertex_data[1] = 0.0f;
        vertex_data[2] = 0.0f;

        vertex_data[3] = 1.0f;
        vertex_data[4] = 0.0f;
        vertex_data[5] = 0.0f;

        vertex_data[6] = 1.0f;
        vertex_data[7] = 1.0f;
        vertex_data[8] = 0.0f;

        vertex_data[9] = 0.0f;
        vertex_data[10] = 1.0f;
        vertex_data[11] = 0.0f;

        vertices->setData(vertex_array);

        // Normals

        QByteArray normal_array;
        normal_array.resize(3*n_vertices*sizeof(float));
        float* normal_data = reinterpret_cast<float*>(normal_array.data());

        normal_data[0] = 0.0f;
        normal_data[1] = 0.0f;
        normal_data[2] = 1.0f;

        normal_data[3] = 0.0f;
        normal_data[4] = 0.0f;
        normal_data[5] = 1.0f;

        normal_data[6] = 0.0f;
        normal_data[7] = 0.0f;
        normal_data[8] = 1.0f;

        normal_data[9] = 0.0f;
        normal_data[10] = 0.0f;
        normal_data[11] = 1.0f;

        normals->setData(normal_array);

         // Indices

        QByteArray index_array;
        index_array.resize(2*3*sizeof(ushort));
        ushort* raw_index_array = reinterpret_cast<ushort *>(index_array.data());

        raw_index_array[0] = 0;
        raw_index_array[1] = 1;
        raw_index_array[2] = 2;

        raw_index_array[3] = 2;
        raw_index_array[4] = 3;
        raw_index_array[5] = 0;

        indices->setData(index_array);
    }
    */

private:
    const static unsigned n_segments = 100; // Todo: Magic number
    const static unsigned n_vertices = 4*(n_segments + 1);
    const static unsigned n_indices = 3*(8*n_segments + 4);     // Three times number of triangles

    Qt3DRender::QBuffer* vertices;
    Qt3DRender::QBuffer* normals;
    Qt3DRender::QBuffer* indices;
};

class InputData;

class GeometryView: public Qt3DExtras::Qt3DWindow
{
public:
    GeometryView(InputData& data)
        : data(data)
    {
        auto *scene = new Qt3DCore::QEntity();

        // Camera
        auto *camera = this->camera();
        //camera->lens()->setOrthographicProjection(-3.0f, 3.0f, -1.0f, 1.0f, 0.1f, 100.0f);
        camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
        camera->setPosition({-0.3f, -0.1f, 0.1f});
        camera->setViewCenter({0.0f, 0.0f, 0.0f});
/*
        camera->setLeft(0.0f);
        camera->setRight(0.8f);
        camera->setBottom(-0.2f);
        camera->setTop(0.2f);
        camera->setNearPlane(0.1f);
        camera->setFarPlane(10.0f);
*/
        // Camera controls
        auto *camController = new Qt3DExtras::QFirstPersonCameraController(scene);  //new Qt3DExtras::QOrbitCameraController(scene);
        camController->setLinearSpeed(2.0f);
        camController->setLookSpeed(100.0f);
        camController->setCamera(camera);

        // Light
        //Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(scene);
        Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(scene);
        light->setColor(0xffffff);
        light->setIntensity(1.0f);
        //lightEntity->addComponent(light);
        //Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
        //lightTransform->setTranslation(camera->position());
        //lightEntity->addComponent(lightTransform);
        camera->addComponent(light);

        // Model
        ModelEntity* model = new ModelEntity(scene);

        this->defaultFrameGraph()->setClearColor(0x4d4d4f);
        this->setRootEntity(scene);

        // Event handling
        // Todo: Use std::bind?
        connections.push_back(data.profile_segments.connect([model, &data](const Series&){ model->update(data); }));
        connections.push_back(      data.profile_x0.connect([model, &data](const double&){ model->update(data); }));
        connections.push_back(      data.profile_y0.connect([model, &data](const double&){ model->update(data); }));
        connections.push_back(    data.profile_phi0.connect([model, &data](const double&){ model->update(data); }));
        connections.push_back(  data.sections_width.connect([model, &data](const Series&){ model->update(data); }));
        connections.push_back( data.sections_height.connect([model, &data](const Series&){ model->update(data); }));
    }

private:
    InputData& data;
    std::vector<Connection> connections;

    void update()
    {
        qInfo() << "Update!";
    }
};
