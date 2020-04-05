#include "LimbView.hpp"
#include "LayerColors.hpp"

#include <QMouseEvent>
#include <QCoreApplication>

LimbView::LimbView()
    : legend(new LayerLegend()),
      limb_mesh_left(true),
      limb_mesh_right(false),
      shader_program(nullptr)
{
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setSamples(32);
    setFormat(format);

    auto button0 = new QToolButton();
    QObject::connect(button0, &QPushButton::clicked, this, &LimbView::viewProfile);
    button0->setIcon(QIcon(":/icons/view-profile"));
    button0->setToolTip("Profile view");
    button0->setIconSize({32, 32});

    auto button1 = new QToolButton();
    QObject::connect(button1, &QPushButton::clicked, this, &LimbView::viewTop);
    button1->setIcon(QIcon(":/icons/view-top"));
    button1->setToolTip("Top view");
    button1->setIconSize({32, 32});

    auto button2 = new QToolButton();
    QObject::connect(button2, &QPushButton::clicked, this, &LimbView::view3D);
    button2->setIcon(QIcon(":/icons/view-3d"));
    button2->setToolTip("Default view");
    button2->setIconSize({32, 32});

    auto button3 = new QToolButton();
    QObject::connect(button3, &QPushButton::clicked, this, &LimbView::viewFit);
    button3->setIcon(QIcon(":/icons/view-fit"));
    button3->setToolTip("Reset zoom");
    button3->setIconSize({32, 32});

    auto button4 = new QToolButton();
    QObject::connect(button4, &QToolButton::toggled, this, &LimbView::viewSymmetric);
    button4->setIcon(QIcon(":/icons/view-symmetric"));
    button4->setToolTip("Show second limb");
    button4->setIconSize({32, 32});
    button4->setCheckable(true);

    auto hbox = new QHBoxLayout();
    hbox->setAlignment(Qt::AlignBottom);
    hbox->addStretch();
    hbox->addWidget(button0);
    hbox->addWidget(button1);
    hbox->addWidget(button2);
    hbox->addWidget(button3);
    hbox->addSpacing(20);
    hbox->addWidget(button4);

    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    vbox->setMargin(25);
    vbox->addWidget(legend);
    vbox->addStretch();
    vbox->addLayout(hbox);

    // Initialize view parameters
    viewSymmetric(false);
    view3D();
}

LimbView::~LimbView()
{
    cleanup();
}

void LimbView::setData(const InputData& data)
{
    legend->setData(data.layers);
    limb_mesh_left.setData(data);
    limb_mesh_right.setData(data);

    limb_mesh_left_vbo.bind();
    limb_mesh_left_vbo.allocate(limb_mesh_left.vertexData().data(), limb_mesh_left.vertexData().size()*sizeof(GLfloat));

    limb_mesh_right_vbo.bind();
    limb_mesh_right_vbo.allocate(limb_mesh_right.vertexData().data(), limb_mesh_right.vertexData().size()*sizeof(GLfloat));

    update();
}

void LimbView::viewProfile()
{
    rot_x = 0.0f;
    rot_y = 0.0f;
    viewFit();
}

void LimbView::viewTop()
{
    rot_x = 90.0f;
    rot_y = 0.0f;
    viewFit();
}

void LimbView::view3D()
{
    rot_x = DEFAULT_ROT_X;
    rot_y = DEFAULT_ROT_Y;
    viewFit();
}

void LimbView::viewSymmetric(bool checked)
{
    limb_mesh_left.setVisible(checked);
    limb_mesh_right.setVisible(true);
    update();
}

void LimbView::viewFit()
{
    shift_x = 0.0f;
    shift_y = 0.0f;
    zoom = DEFAULT_ZOOM;
    update();
}

void LimbView::cleanup()
{
    if(shader_program == nullptr)
        return;

    makeCurrent();
    limb_mesh_left_vbo.destroy();
    limb_mesh_right_vbo.destroy();
    delete shader_program;
    shader_program = nullptr;
    doneCurrent();
}

static const char *vertexShaderSource =
    "#version 120\n"
    "attribute vec3 modelPosition;\n"
    "attribute vec3 modelNormal;\n"
    "attribute vec3 modelColor;\n"
    "varying vec3 vertexPosition;\n"
    "varying vec3 vertexNormal;\n"
    "varying vec3 vertexColor;\n"
    "uniform mat4 projectionMatrix;\n"
    "uniform mat4 modelViewMatrix;\n"
    "uniform mat3 normalMatrix;\n"
    "void main() {\n"
    "   vertexPosition = modelPosition;\n"
    "   vertexNormal = normalMatrix*modelNormal;\n"
    "   vertexColor = modelColor;\n"
    "   gl_Position = projectionMatrix*modelViewMatrix*vec4(modelPosition.xyz, 1.0);\n"
    "}\n";

static const char *fragmentShaderSource =
    "#version 120\n"
    "varying highp vec3 vertexPosition;\n"
    "varying highp vec3 vertexNormal;\n"
    "varying highp vec3 vertexColor;\n"
    "uniform highp vec3 lightPosition;\n"
    "uniform highp vec3 cameraPosition;\n"
    "uniform highp float ambientStrength;\n"
    "uniform highp float diffuseStrength;\n"
    "uniform highp float specularStrength;\n"
    "uniform highp float materialShininess;\n"
    "void main() {\n"
    "   highp vec3 normalDirection = normalize(vertexNormal);\n"
    "   highp vec3 lightDirection = normalize(lightPosition - vertexPosition);\n"
    "   highp vec3 viewDirection = normalize(cameraPosition - vertexPosition);\n"
    "   highp vec3 reflectDirection = reflect(-lightDirection, normalDirection);\n"
    "   highp float diffuse = max(dot(lightDirection, normalDirection), 0.0);\n"
    "   highp float specular = pow(max(dot(viewDirection, reflectDirection), 0.0), materialShininess);\n"
    "   highp vec3 result = (ambientStrength + diffuseStrength*diffuse + specularStrength*specular)*vertexColor;\n"
    "   gl_FragColor = vec4(result, 1.0);\n"
    "}\n";

void LimbView::initializeGL()
{
    // Comment from the Qt "Hello GL2" example:
    // In this example the widget's corresponding top-level window can change several times during the widget's lifetime.
    // Whenever this happens, the QOpenGLWidget's associated context is destroyed and a new one is created.
    // Therefore we have to be prepared to clean up the resources on the aboutToBeDestroyed() signal, instead of the destructor.
    // The emission of the signal will be followed by an invocation of initializeGL() where we can recreate all resources.
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &LimbView::cleanup);

    initializeOpenGLFunctions();
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    glLineWidth(2.0f);

    shader_program = new QOpenGLShaderProgram;
    shader_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    shader_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    shader_program->bindAttributeLocation("modelPosition", 0);
    shader_program->bindAttributeLocation("modelNormal", 1);
    shader_program->bindAttributeLocation("modelColor", 2);
    shader_program->link();

    shader_program->bind();
    loc_projectionMatrix = shader_program->uniformLocation("projectionMatrix");
    loc_modelViewMatrix = shader_program->uniformLocation("modelViewMatrix");
    loc_normalMatrix = shader_program->uniformLocation("normalMatrix");
    loc_lightPosition = shader_program->uniformLocation("lightPosition");
    loc_cameraPosition = shader_program->uniformLocation("cameraPosition");
    loc_materialAmbient = shader_program->uniformLocation("ambientStrength");
    loc_materialDiffuse = shader_program->uniformLocation("diffuseStrength");
    loc_materialSpecular = shader_program->uniformLocation("specularStrength");
    loc_materialShininess = shader_program->uniformLocation("materialShininess");

    shader_program->setUniformValue(loc_lightPosition, LIGHT_POSITION);
    shader_program->setUniformValue(loc_cameraPosition, CAMERA_POSITION);
    shader_program->setUniformValue(loc_materialAmbient, MATERIAL_AMBIENT);
    shader_program->setUniformValue(loc_materialDiffuse, MATERIAL_DIFFUSE);
    shader_program->setUniformValue(loc_materialSpecular, MATERIAL_SPECULAR);
    shader_program->setUniformValue(loc_materialShininess, MATERIAL_SHININESS);
    shader_program->release();

    // Setup vertex buffer object for left limb
    limb_mesh_left_vbo.create();
    limb_mesh_left_vbo.bind();
    limb_mesh_left_vbo.allocate(limb_mesh_left.vertexData().data(), limb_mesh_left.vertexData().size()*sizeof(GLfloat));
    limb_mesh_left_vbo.release();

    // Setup vertex buffer object for right limb
    limb_mesh_right_vbo.create();
    limb_mesh_right_vbo.bind();
    limb_mesh_right_vbo.allocate(limb_mesh_right.vertexData().data(), limb_mesh_right.vertexData().size()*sizeof(GLfloat));
    limb_mesh_right_vbo.release();
}

void LimbView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);

    AABB content_bounds;
    if(limb_mesh_left.isVisible())
        content_bounds = content_bounds.extend(limb_mesh_left.aabb());
    if(limb_mesh_right.isVisible())
        content_bounds = content_bounds.extend(limb_mesh_right.aabb());

    m_world.setToIdentity();
    m_world.rotate(rot_x, 1.0f, 0.0f, 0.0f);
    m_world.rotate(rot_y, 0.0f, 1.0f, 0.0f);
    m_world.scale(1.0f/content_bounds.diagonal());
    m_world.translate(-content_bounds.center());
    m_camera.setToIdentity();
    m_camera.translate(CAMERA_POSITION);

    float aspect_ratio = float(this->height())/this->width();
    m_projection.setToIdentity();
    m_projection.ortho((-0.5f*zoom + shift_x),
                       ( 0.5f*zoom + shift_x),
                       (-0.5f*zoom + shift_y)*aspect_ratio,
                       ( 0.5f*zoom + shift_y)*aspect_ratio,
                       0.001f, 100.0f);

    shader_program->bind();
    shader_program->setUniformValue(loc_projectionMatrix, m_projection);
    shader_program->setUniformValue(loc_modelViewMatrix, m_camera*m_world);
    shader_program->setUniformValue(loc_normalMatrix, m_world.normalMatrix());

    if(limb_mesh_left.isVisible())
    {
        limb_mesh_left_vbo.bind();
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (void*)(0*sizeof(GLfloat)));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (void*)(6*sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glDrawArrays(GL_TRIANGLES, 0, limb_mesh_left.vertexCount());
        limb_mesh_left_vbo.release();
    }

    if(limb_mesh_right.isVisible())
    {
        limb_mesh_right_vbo.bind();
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (void*)(0*sizeof(GLfloat)));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (void*)(6*sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glDrawArrays(GL_TRIANGLES, 0, limb_mesh_right.vertexCount());
        limb_mesh_right_vbo.release();
    }

    shader_program->release();
}

void LimbView::mousePressEvent(QMouseEvent *event)
{
    mouse_pos = event->pos();
}

void LimbView::mouseMoveEvent(QMouseEvent *event)
{
    int delta_x = event->x() - mouse_pos.x();
    int delta_y = event->y() - mouse_pos.y();

    if(event->buttons() & Qt::LeftButton)
    {
        rot_x += ROT_SPEED*delta_y;
        rot_y += ROT_SPEED*delta_x;
        update();
    }
    else if(event->buttons() & Qt::MiddleButton)
    {
        shift_x -= float(delta_x)/this->width()*zoom;
        shift_y += float(delta_y)/this->height()*zoom;
        update();
    }

    mouse_pos = event->pos();
}

void LimbView::wheelEvent(QWheelEvent* event)
{
    float delta_zoom = -ZOOM_SPEED*event->angleDelta().y()/120.0f*zoom;    // Dividing by 120 gives the number of 15 degree steps on a standard mouse
    float mouse_ratio_x = float(event->x())/this->width();
    float mouse_ratio_y = float(event->y())/this->height();

    shift_x -= (mouse_ratio_x - 0.5f)*delta_zoom;
    shift_y += (mouse_ratio_y - 0.5f)*delta_zoom;
    zoom += delta_zoom;

    update();
}
