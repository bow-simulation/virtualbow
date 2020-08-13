#include "LimbView.hpp"
#include "LayerColors.hpp"
#include <QMouseEvent>
#include <QCoreApplication>

LimbView::LimbView()
    : legend(new LayerLegend()),
      limb_mesh_left(true),
      limb_mesh_right(false),
      background_shader(nullptr)
{
    // Anti aliasing
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setSamples(8);
    setFormat(format);

    const QSize BUTTON_SIZE = {34, 34};
    const QString BUTTON_STYLE = "QToolButton { background-color: rgba(220, 220, 220, 200); border-radius: 5px; }"
                                 "QToolButton:pressed { background-color: rgba(150, 150, 150, 200); }"
                                 "QToolButton:checked { background-color: rgba(150, 150, 150, 200); }";

    auto button0 = new QToolButton();
    QObject::connect(button0, &QPushButton::clicked, this, &LimbView::view3D);
    button0->setIcon(QIcon(":/icons/view-3d"));
    button0->setToolTip("Default view");
    button0->setIconSize(BUTTON_SIZE);
    button0->setStyleSheet(BUTTON_STYLE);

    auto button1 = new QToolButton();
    QObject::connect(button1, &QPushButton::clicked, this, &LimbView::viewProfile);
    button1->setIcon(QIcon(":/icons/view-profile"));
    button1->setToolTip("Profile view");
    button1->setIconSize(BUTTON_SIZE);
    button1->setStyleSheet(BUTTON_STYLE);

    auto button2 = new QToolButton();
    QObject::connect(button2, &QPushButton::clicked, this, &LimbView::viewTop);
    button2->setIcon(QIcon(":/icons/view-top"));
    button2->setToolTip("Top view");
    button2->setIconSize(BUTTON_SIZE);
    button2->setStyleSheet(BUTTON_STYLE);

    auto button3 = new QToolButton();
    QObject::connect(button3, &QPushButton::clicked, this, &LimbView::viewFit);
    button3->setIcon(QIcon(":/icons/view-fit"));
    button3->setToolTip("Reset zoom");
    button3->setIconSize(BUTTON_SIZE);
    button3->setStyleSheet(BUTTON_STYLE);

    auto button4 = new QToolButton();
    QObject::connect(button4, &QToolButton::toggled, this, &LimbView::viewSymmetric);
    button4->setIcon(QIcon(":/icons/view-symmetric"));
    button4->setToolTip("Show symmetry");
    button4->setIconSize(BUTTON_SIZE);
    button4->setStyleSheet(BUTTON_STYLE);
    button4->setCheckable(true);

    auto vbox1 = new QVBoxLayout();
    vbox1->setAlignment(Qt::AlignLeft);
    vbox1->addWidget(legend);
    vbox1->addStretch();

    auto vbox2 = new QVBoxLayout();
    vbox2->setAlignment(Qt::AlignRight);
    vbox2->setSpacing(8);
    vbox2->addWidget(button0);
    vbox2->addWidget(button1);
    vbox2->addWidget(button2);
    vbox2->addWidget(button3);
    vbox2->addWidget(button4);
    vbox2->addStretch();

    auto hbox = new QHBoxLayout();
    this->setLayout(hbox);
    hbox->setMargin(20);
    hbox->addLayout(vbox1);
    hbox->addStretch();
    hbox->addLayout(vbox2);

    // Initialize view parameters
    viewSymmetric(false);
    view3D();
}

void LimbView::setData(const InputData& data)
{
    legend->setData(data.layers);
    limb_mesh_left.setData(data);
    limb_mesh_right.setData(data);

    /*
    limb_mesh_left_vbo.bind();
    limb_mesh_left_vbo.allocate(limb_mesh_left.vertexData().data(), limb_mesh_left.vertexData().size()*sizeof(GLfloat));

    limb_mesh_right_vbo.bind();
    limb_mesh_right_vbo.allocate(limb_mesh_right.vertexData().data(), limb_mesh_right.vertexData().size()*sizeof(GLfloat));
    */

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

void LimbView::initializeGL()
{
    initializeOpenGLFunctions();

    // Shaders

    background_shader = new QOpenGLShaderProgram(this);
    background_shader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/BackgroundShader.vs");
    background_shader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/BackgroundShader.fs");
    background_shader->link();

    model_shader = new QOpenGLShaderProgram(this);
    model_shader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/ModelShader.vs");
    model_shader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/ModelShader.fs");
    model_shader->link();
    model_shader->bind();
    model_shader->setUniformValue("lightColor", LIGHT_COLOR);
    model_shader->setUniformValue("lightPosition", LIGHT_POSITION);
    model_shader->setUniformValue("cameraPosition", CAMERA_POSITION);
    model_shader->setUniformValue("ambientStrength", MATERIAL_AMBIENT_STRENGTH);
    model_shader->setUniformValue("diffuseStrength", MATERIAL_DIFFUSE_STRENGTH);
    model_shader->setUniformValue("specularStrength", MATERIAL_SPECULAR_STRENGTH);
    model_shader->setUniformValue("materialShininess", MATERIAL_SHININESS);
    model_shader->release();

    // Create background mesh

    Mesh background_mesh;
    unsigned i0 = background_mesh.addVertex({ 1.0f, -1.0f, 0.0f}, { 0.0f, 0.0f, 1.0f }, BACKGROUND_COLOR_1);
    unsigned i1 = background_mesh.addVertex({-1.0f, -1.0f, 0.0f}, { 0.0f, 0.0f, 1.0f }, BACKGROUND_COLOR_1);
    unsigned i2 = background_mesh.addVertex({-1.0f,  1.0f, 0.0f}, { 0.0f, 0.0f, 1.0f }, BACKGROUND_COLOR_2);
    unsigned i3 = background_mesh.addVertex({ 1.0f,  1.0f, 0.0f}, { 0.0f, 0.0f, 1.0f }, BACKGROUND_COLOR_2);
    background_mesh.addTriangle(i0, i2, i1);
    background_mesh.addTriangle(i2, i0, i3);

    Model background(background_mesh, background_shader);
    scene_objects.push_back(background);

    // Create cube mesh
    // Right
    unsigned j0 = cube_mesh.addVertex(0.4*QVector3D{ 1.0, -1.0,  1.0 }, QVector3D{ 1.0, 0.0, 0.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    unsigned j1 = cube_mesh.addVertex(0.4*QVector3D{ 1.0, -1.0, -1.0 }, QVector3D{ 1.0, 0.0, 0.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    unsigned j2 = cube_mesh.addVertex(0.4*QVector3D{ 1.0,  1.0, -1.0 }, QVector3D{ 1.0, 0.0, 0.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    unsigned j3 = cube_mesh.addVertex(0.4*QVector3D{ 1.0,  1.0,  1.0 }, QVector3D{ 1.0, 0.0, 0.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    cube_mesh.addTriangle(j0, j1, j3);
    cube_mesh.addTriangle(j1, j2, j3);

    // Left
    j0 = cube_mesh.addVertex(0.4*QVector3D{-1.0, -1.0, -1.0 }, QVector3D{-1.0, 0.0, 0.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    j1 = cube_mesh.addVertex(0.4*QVector3D{-1.0, -1.0,  1.0 }, QVector3D{-1.0, 0.0, 0.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    j2 = cube_mesh.addVertex(0.4*QVector3D{-1.0,  1.0,  1.0 }, QVector3D{-1.0, 0.0, 0.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    j3 = cube_mesh.addVertex(0.4*QVector3D{-1.0,  1.0, -1.0 }, QVector3D{-1.0, 0.0, 0.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    cube_mesh.addTriangle(j0, j1, j3);
    cube_mesh.addTriangle(j1, j2, j3);

    // Top
    j0 = cube_mesh.addVertex(0.4*QVector3D{-1.0,  1.0,  1.0 }, QVector3D{ 0.0, 1.0, 0.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    j1 = cube_mesh.addVertex(0.4*QVector3D{ 1.0,  1.0,  1.0 }, QVector3D{ 0.0, 1.0, 0.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    j2 = cube_mesh.addVertex(0.4*QVector3D{ 1.0,  1.0, -1.0 }, QVector3D{ 0.0, 1.0, 0.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    j3 = cube_mesh.addVertex(0.4*QVector3D{-1.0,  1.0, -1.0 }, QVector3D{ 0.0, 1.0, 0.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    cube_mesh.addTriangle(j0, j1, j3);
    cube_mesh.addTriangle(j1, j2, j3);

    // Bottom
    j0 = cube_mesh.addVertex(0.4*QVector3D{-1.0, -1.0, -1.0 }, QVector3D{ 0.0, -1.0, 0.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    j1 = cube_mesh.addVertex(0.4*QVector3D{ 1.0, -1.0, -1.0 }, QVector3D{ 0.0, -1.0, 0.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    j2 = cube_mesh.addVertex(0.4*QVector3D{ 1.0, -1.0,  1.0 }, QVector3D{ 0.0, -1.0, 0.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    j3 = cube_mesh.addVertex(0.4*QVector3D{-1.0, -1.0,  1.0 }, QVector3D{ 0.0, -1.0, 0.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    cube_mesh.addTriangle(j0, j1, j3);
    cube_mesh.addTriangle(j1, j2, j3);

    // Front
    j0 = cube_mesh.addVertex(0.4*QVector3D{-1.0, -1.0, 1.0 }, QVector3D{ 0.0, 0.0, 1.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    j1 = cube_mesh.addVertex(0.4*QVector3D{ 1.0, -1.0, 1.0 }, QVector3D{ 0.0, 0.0, 1.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    j2 = cube_mesh.addVertex(0.4*QVector3D{ 1.0,  1.0, 1.0 }, QVector3D{ 0.0, 0.0, 1.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    j3 = cube_mesh.addVertex(0.4*QVector3D{-1.0,  1.0, 1.0 }, QVector3D{ 0.0, 0.0, 1.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    cube_mesh.addTriangle(j0, j1, j3);
    cube_mesh.addTriangle(j1, j2, j3);

    // Back
    j0 = cube_mesh.addVertex(0.4*QVector3D{ 1.0, -1.0, -1.0 }, QVector3D{ 0.0, 0.0, -1.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    j1 = cube_mesh.addVertex(0.4*QVector3D{-1.0, -1.0, -1.0 }, QVector3D{ 0.0, 0.0, -1.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    j2 = cube_mesh.addVertex(0.4*QVector3D{-1.0,  1.0, -1.0 }, QVector3D{ 0.0, 0.0, -1.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    j3 = cube_mesh.addVertex(0.4*QVector3D{ 1.0,  1.0, -1.0 }, QVector3D{ 0.0, 0.0, -1.0 }, QColor::fromRgbF(1.0f, 0.5f, 0.31f));
    cube_mesh.addTriangle(j0, j1, j3);
    cube_mesh.addTriangle(j1, j2, j3);


    Model cube(cube_mesh, model_shader);
    scene_objects.push_back(cube);

}

void LimbView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //glEnable(GL_MULTISAMPLE);

    AABB bounds({-1.0, -1.0, -1.0}, {1.0, 1.0, 1.0});

    QMatrix4x4 m_model;
    m_model.setToIdentity();
    m_model.rotate(rot_x, 1.0f, 0.0f, 0.0f);
    m_model.rotate(rot_y, 0.0f, 1.0f, 0.0f);
    m_model.scale(1.0f/bounds.diagonal());
    m_model.translate(-bounds.center());

    QMatrix4x4 m_view;
    m_view.setToIdentity();
    //m_view.translate(CAMERA_POSITION);
    m_view.lookAt(CAMERA_POSITION, bounds.center(), {0.0, 1.0, 0.0});

    float aspect_ratio = float(this->height())/this->width();
    QMatrix4x4 m_projection;
    m_projection.setToIdentity();
    m_projection.ortho((-0.5f*zoom + shift_x),
                       ( 0.5f*zoom + shift_x),
                       (-0.5f*zoom + shift_y)*aspect_ratio,
                       ( 0.5f*zoom + shift_y)*aspect_ratio,
                       0.001f, 100.0f);

    model_shader->bind();
    model_shader->setUniformValue("modelMatrix", m_model);
    model_shader->setUniformValue("normalMatrix", m_model.normalMatrix());
    model_shader->setUniformValue("viewMatrix", m_view);
    model_shader->setUniformValue("projectionMatrix", m_projection);
    model_shader->release();

    glClear(GL_COLOR_BUFFER_BIT);
    for(auto& object: scene_objects) {
        object.draw();
    }
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
