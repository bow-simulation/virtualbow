#include "LimbView.hpp"
#include "LayerColors.hpp"

#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>

LimbView::LimbView()
    : rot_x(0.0f),
      rot_y(0.0f),
      zoom(1.0f),
      shift_x(0.0f),
      shift_y(0.0f),
      m_program(nullptr)
{
    auto button0 = new QToolButton();
    QObject::connect(button0, &QPushButton::clicked, this, &LimbView::viewProfile);
    button0->setIcon(QIcon(":/icons/limb-view/view-profile"));
    button0->setToolTip("Profile view");
    button0->setIconSize({32, 32});

    auto button1 = new QToolButton();
    QObject::connect(button1, &QPushButton::clicked, this, &LimbView::viewTop);
    button1->setIcon(QIcon(":/icons/limb-view/view-top"));
    button1->setToolTip("Top view");
    button1->setIconSize({32, 32});

    auto button2 = new QToolButton();
    QObject::connect(button2, &QPushButton::clicked, this, &LimbView::view3D);
    button2->setIcon(QIcon(":/icons/limb-view/view-3d"));
    button2->setToolTip("3D view");
    button2->setIconSize({32, 32});

    auto button3 = new QToolButton();
    QObject::connect(button3, &QPushButton::clicked, this, &LimbView::viewFit);
    button3->setIcon(QIcon(":/icons/limb-view/view-fit"));
    button3->setToolTip("Fit view");
    button3->setIconSize({32, 32});

    auto button4 = new QToolButton();
    QObject::connect(button4, &QToolButton::toggled, this, &LimbView::viewSymmetric);
    button4->setIcon(QIcon(":/icons/limb-view/view-symmetric"));
    button4->setToolTip("Show complete bow");
    button4->setIconSize({32, 32});
    button4->setCheckable(true);

    auto hbox = new QHBoxLayout();
    hbox->setAlignment(Qt::AlignBottom);
    hbox->setMargin(15);
    hbox->addStretch();
    hbox->addWidget(button0);
    hbox->addWidget(button1);
    hbox->addWidget(button2);
    hbox->addWidget(button3);
    hbox->addSpacing(20);
    hbox->addWidget(button4);
    this->setLayout(hbox);

    view3D();
}

LimbView::~LimbView()
{
    cleanup();
}

void LimbView::setData(const InputData& data)
{
    m_mesh.setData(data);

    m_meshVbo.create();
    m_meshVbo.bind();
    m_meshVbo.allocate(m_mesh.vertexData().data(), m_mesh.vertexData().size()*sizeof(GLfloat));

    update();
}

void LimbView::viewProfile()
{

}

void LimbView::viewTop()
{

}

void LimbView::view3D()
{

}

void LimbView::viewSymmetric(bool checked)
{

}

void LimbView::viewFit()
{

}

void LimbView::cleanup()
{
    if(m_program == nullptr)
        return;

    makeCurrent();
    m_meshVbo.destroy();
    delete m_program;
    m_program = nullptr;
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
    "void main() {\n"
    "   highp vec3 L = normalize(lightPosition - vertexPosition);\n"
    "   highp float NL = max(dot(normalize(vertexNormal), L), 0.0);\n"
    "   highp vec3 color = clamp(vertexColor*0.2 + vertexColor*0.8*NL, 0.0, 1.0);\n"
    "   gl_FragColor = vec4(color, 1.0);\n"
    "}\n";

void LimbView::initializeGL()
{
    // In this example the widget's corresponding top-level window can change
    // several times during the widget's lifetime. Whenever this happens, the
    // QOpenGLWidget's associated context is destroyed and a new one is created.
    // Therefore we have to be prepared to clean up the resources on the
    // aboutToBeDestroyed() signal, instead of the destructor. The emission of
    // the signal will be followed by an invocation of initializeGL() where we
    // can recreate all resources.
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &LimbView::cleanup);

    initializeOpenGLFunctions();
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);

    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->bindAttributeLocation("modelPosition", 0);
    m_program->bindAttributeLocation("modelNormal", 1);
    m_program->bindAttributeLocation("modelColor", 2);
    m_program->link();

    m_program->bind();
    m_projectionMatrixLoc = m_program->uniformLocation("projectionMatrix");
    m_modelViewMatrixLoc = m_program->uniformLocation("modelViewMatrix");
    m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
    m_lightPositionLoc = m_program->uniformLocation("lightPosition");

    // Setup our vertex buffer object.
    m_meshVbo.create();
    m_meshVbo.bind();
    m_meshVbo.allocate(m_mesh.vertexData().data(), m_mesh.vertexData().size()*sizeof(GLfloat));

    // Store the vertex attribute bindings for the program.
    m_meshVbo.bind();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (void*)(0*sizeof(GLfloat)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (void*)(6*sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    m_meshVbo.release();

    // Light position is fixed.
    m_program->setUniformValue(m_lightPositionLoc, QVector3D(0.0f, 0.0f, 50.0f));
    m_program->release();
}

void LimbView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    m_world.setToIdentity();
    m_world.rotate(180.0f - rot_x, 1.0f, 0.0f, 0.0f);
    m_world.rotate(rot_y, 0.0f, 1.0f, 0.0f);
    m_world.scale(1.0f/m_mesh.aabbDiagonal());
    m_world.translate(-m_mesh.aabbCenter());

    m_camera.setToIdentity();
    m_camera.translate(0.0f, 0.0f, -1.0f);

    float aspect_ratio = float(this->height())/this->width();
    m_projection.setToIdentity();
    m_projection.ortho((-0.5f*zoom + shift_x),
                       ( 0.5f*zoom + shift_x),
                       (-0.5f*zoom + shift_y)*aspect_ratio,
                       ( 0.5f*zoom + shift_y)*aspect_ratio,
                       0.001f, 100.0f);

    m_program->bind();
    m_program->setUniformValue(m_projectionMatrixLoc, m_projection);
    m_program->setUniformValue(m_modelViewMatrixLoc, m_camera*m_world);
    m_program->setUniformValue(m_normalMatrixLoc, m_world.normalMatrix());

    glDrawArrays(GL_TRIANGLES, 0, m_mesh.vertexCount());
    m_program->release();
}

void LimbView::mousePressEvent(QMouseEvent *event)
{
    last_mouse_pos = event->pos();
}

void LimbView::mouseMoveEvent(QMouseEvent *event)
{
    int delta_x = event->x() - last_mouse_pos.x();
    int delta_y = event->y() - last_mouse_pos.y();

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

    last_mouse_pos = event->pos();
}

void LimbView::wheelEvent(QWheelEvent* event)
{
    float delta_zoom = -ZOOM_SPEED*event->angleDelta().y()/120.0f*zoom;    // dividing by 120 gives the number of 15 degree steps on a standard mouse
    float mouse_ratio_x = float(event->x())/this->width();
    float mouse_ratio_y = float(event->y())/this->height();

    shift_x -= (mouse_ratio_x - 0.5f)*delta_zoom;
    shift_y += (mouse_ratio_y - 0.5f)*delta_zoom;
    zoom += delta_zoom;

    update();
}
