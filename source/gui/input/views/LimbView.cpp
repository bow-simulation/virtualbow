#include "LimbView.hpp"
#include "LayerColors.hpp"

#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>

LimbView::LimbView()
    : m_xRot(0),
      m_yRot(0),
      m_zRot(0),
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

static void normalizeAngle(int &angle)
{
    while(angle < 0)
        angle += 360 * 16;

    while(angle > 360 * 16)
        angle -= 360 * 16;
}

void LimbView::setXRotation(int angle)
{
    normalizeAngle(angle);
    m_xRot = angle;
    update();
}

void LimbView::setYRotation(int angle)
{
    normalizeAngle(angle);
    m_yRot = angle;
    update();
}

void LimbView::setZRotation(int angle)
{
    normalizeAngle(angle);
    m_zRot = angle;
    update();
}

void LimbView::cleanup()
{
    if(m_program == nullptr)
        return;

    makeCurrent();
    m_logoVbo.destroy();
    delete m_program;
    m_program = nullptr;
    doneCurrent();
}

static const char *vertexShaderSource =
    "attribute vec4 vertex;\n"
    "attribute vec3 normal;\n"
    "varying vec3 vert;\n"
    "varying vec3 vertNormal;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "uniform mat3 normalMatrix;\n"
    "void main() {\n"
    "   vert = vertex.xyz;\n"
    "   vertNormal = normalMatrix * normal;\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying highp vec3 vert;\n"
    "varying highp vec3 vertNormal;\n"
    "uniform highp vec3 lightPos;\n"
    "void main() {\n"
    "   highp vec3 L = normalize(lightPos - vert);\n"
    "   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
    "   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
    "   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
    "   gl_FragColor = vec4(col, 1.0);\n"
    "}\n";

void LimbView::setupVertexAttribs()
{
    m_logoVbo.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    m_logoVbo.release();
}

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
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->link();

    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
    m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
    m_lightPosLoc = m_program->uniformLocation("lightPos");

    // Create a vertex array object. In OpenGL ES 2.0 and OpenGL 2.x
    // implementations this is optional and support may not be present
    // at all. Nonetheless the below code works in all cases and makes
    // sure there is a VAO when one is needed.
    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // Setup our vertex buffer object.
    m_logoVbo.create();
    m_logoVbo.bind();
    m_logoVbo.allocate(m_logo.constData(), m_logo.count() * sizeof(GLfloat));

    // Store the vertex attribute bindings for the program.
    setupVertexAttribs();

    // Our camera never changes in this example.
    m_camera.setToIdentity();
    m_camera.translate(0, 0, -1);

    // Light position is fixed.
    m_program->setUniformValue(m_lightPosLoc, QVector3D(0, 0, 70));

    m_program->release();
}

void LimbView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    m_world.setToIdentity();
    m_world.rotate(180.0f - (m_xRot / 16.0f), 1, 0, 0);
    m_world.rotate(m_yRot / 16.0f, 0, 1, 0);
    m_world.rotate(m_zRot / 16.0f, 0, 0, 1);

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, m_proj);
    m_program->setUniformValue(m_mvMatrixLoc, m_camera * m_world);
    QMatrix3x3 normalMatrix = m_world.normalMatrix();
    m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);

    glDrawArrays(GL_TRIANGLES, 0, m_logo.vertexCount());

    m_program->release();
}

void LimbView::resizeGL(int width, int height)
{
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(width)/height, 0.01f, 100.0f);
}

void LimbView::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void LimbView::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if(event->buttons() & Qt::LeftButton)
    {
        setXRotation(m_xRot + 8 * dy);
        setYRotation(m_yRot + 8 * dx);
    }
    else if(event->buttons() & Qt::RightButton)
    {
        setXRotation(m_xRot + 8 * dy);
        setZRotation(m_zRot + 8 * dx);
    }

    m_lastPos = event->pos();
}
