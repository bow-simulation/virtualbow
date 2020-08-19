#include "LimbView.hpp"
#include "LayerColors.hpp"
#include <QMouseEvent>
#include <QCoreApplication>

LimbView::LimbView()
    : legend(new LayerLegend()),
      background_shader(nullptr),
      model_shader(nullptr),
      edge_shader(nullptr)
{
    // Anti aliasing
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setSamples(8);
    setFormat(format);

    const QSize BUTTON_SIZE = {34, 34};
    const QString BUTTON_STYLE = "QToolButton { background-color: rgba(220, 220, 220, 200); border-radius: 5px; }"
                                 "QToolButton:pressed { background-color: rgba(150, 150, 150, 200); }"
                                 "QToolButton:checked { background-color: rgba(150, 150, 150, 200); }";

    auto bt_view_3d = new QToolButton();
    QObject::connect(bt_view_3d, &QPushButton::clicked, this, &LimbView::view3D);
    bt_view_3d->setIcon(QIcon(":/icons/view-3d"));
    bt_view_3d->setToolTip("Default view");
    bt_view_3d->setIconSize(BUTTON_SIZE);
    bt_view_3d->setStyleSheet(BUTTON_STYLE);

    auto bt_view_profile = new QToolButton();
    QObject::connect(bt_view_profile, &QPushButton::clicked, this, &LimbView::viewProfile);
    bt_view_profile->setIcon(QIcon(":/icons/view-profile"));
    bt_view_profile->setToolTip("Profile view");
    bt_view_profile->setIconSize(BUTTON_SIZE);
    bt_view_profile->setStyleSheet(BUTTON_STYLE);

    auto bt_view_top = new QToolButton();
    QObject::connect(bt_view_top, &QPushButton::clicked, this, &LimbView::viewTop);
    bt_view_top->setIcon(QIcon(":/icons/view-top"));
    bt_view_top->setToolTip("Top view");
    bt_view_top->setIconSize(BUTTON_SIZE);
    bt_view_top->setStyleSheet(BUTTON_STYLE);

    auto bt_view_fit = new QToolButton();
    QObject::connect(bt_view_fit, &QPushButton::clicked, this, &LimbView::viewFit);
    bt_view_fit->setIcon(QIcon(":/icons/view-fit"));
    bt_view_fit->setToolTip("Reset zoom");
    bt_view_fit->setIconSize(BUTTON_SIZE);
    bt_view_fit->setStyleSheet(BUTTON_STYLE);

    auto bt_view_symmetric = new QToolButton();
    QObject::connect(bt_view_symmetric, &QToolButton::toggled, this, &LimbView::viewSymmetric);
    bt_view_symmetric->setIcon(QIcon(":/icons/view-symmetric"));
    bt_view_symmetric->setToolTip("Show symmetry");
    bt_view_symmetric->setIconSize(BUTTON_SIZE);
    bt_view_symmetric->setStyleSheet(BUTTON_STYLE);
    bt_view_symmetric->setCheckable(true);

    auto vbox1 = new QVBoxLayout();
    vbox1->setAlignment(Qt::AlignLeft);
    vbox1->addWidget(legend);
    vbox1->addStretch();

    auto vbox2 = new QVBoxLayout();
    vbox2->setAlignment(Qt::AlignRight);
    vbox2->setSpacing(8);
    vbox2->addWidget(bt_view_3d);
    vbox2->addWidget(bt_view_profile);
    vbox2->addWidget(bt_view_top);
    vbox2->addWidget(bt_view_fit);
    vbox2->addWidget(bt_view_symmetric);
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

    LimbMesh mesh(data);
    limb_faces_right = std::make_unique<Model>(mesh.faces_right);
    limb_edges_right = std::make_unique<Model>(mesh.edges_right);
    limb_faces_left = std::make_unique<Model>(mesh.faces_left);
    limb_edges_left = std::make_unique<Model>(mesh.edges_left);

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
    symmetry = checked;
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

    // OpenGL configuration

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glLineWidth(1.0f);

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
    model_shader->setUniformValue("cameraPosition", CAMERA_POSITION);
    model_shader->setUniformValue("lightPosition", LIGHT_POSITION);
    model_shader->setUniformValue("lightColor", LIGHT_COLOR);
    model_shader->setUniformValue("ambientStrength", MATERIAL_AMBIENT_STRENGTH);
    model_shader->setUniformValue("diffuseStrength", MATERIAL_DIFFUSE_STRENGTH);
    model_shader->setUniformValue("specularStrength", MATERIAL_SPECULAR_STRENGTH);
    model_shader->setUniformValue("materialShininess", MATERIAL_SHININESS);
    model_shader->release();

    edge_shader = new QOpenGLShaderProgram(this);
    edge_shader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/EdgeShader.vs");
    edge_shader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/EdgeShader.fs");
    edge_shader->link();

    // Create background mesh

    Mesh background_mesh(GL_QUADS);
    QColor COLOR_1 = BACKGROUND_COLOR_1;
    QColor COLOR_2 = BACKGROUND_COLOR_2;
    QDate date = QDate::currentDate();
    if(date.month() == 12 && (date.day() == 24 || date.day() == 25 || date.day() == 26)) {
        auto create_star = [&](float x0, float y0, float r, float R, float alpha, unsigned n) {
            float beta = 2.0*M_PI/n;
            float z0 = -0.1;
            for(unsigned i = 0; i < n; ++i) {
                float phi = alpha + i*beta;
                QVector3D p0(x0, y0, z0);
                QVector3D p1(x0 - r*sin(phi - beta/2), y0 + r*cos(phi - beta/2), z0);
                QVector3D p2(x0 - R*sin(phi), y0 + R*cos(phi), z0);
                QVector3D p3(x0 - r*sin(phi + beta/2), y0 + r*cos(phi + beta/2), z0);
                background_mesh.addQuad(p0, p1, p2, p3, QColor::fromRgb(255, 255, 255));
            }
        };

        for(unsigned i = 0; i < 15; ++i) {
            float x = -1.0 + static_cast<float>(rand())/static_cast <float> (RAND_MAX/(2.0));
            float y = -1.0 + static_cast<float>(rand())/static_cast <float> (RAND_MAX/(2.0));
            float r = 0.002 + static_cast<float>(rand())/static_cast <float> (RAND_MAX/(0.004));
            create_star(x, y, r, 3.0*r, 0.0, 4);
        }

        COLOR_1 = QColor::fromRgbF(0.6f, 0.3f, 0.4f);
        COLOR_2 = QColor::fromRgbF(0.2f, 0.3f, 0.4f);
    }
    background_mesh.addVertex({ 1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, COLOR_1);
    background_mesh.addVertex({-1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, COLOR_1);
    background_mesh.addVertex({-1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, COLOR_2);
    background_mesh.addVertex({ 1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, COLOR_2);
    background = std::make_unique<Model>(background_mesh);
}

void LimbView::paintGL()
{
    Bounds bounds = limb_faces_right->getBounds();
    if(symmetry) {
        bounds.extend(limb_faces_left->getBounds());
    }

    qInfo() << bounds.diagonal();
    qInfo() << bounds.center();

    QMatrix4x4 m_model;
    m_model.setToIdentity();
    m_model.rotate(rot_x, 1.0f, 0.0f, 0.0f);
    m_model.rotate(rot_y, 0.0f, 1.0f, 0.0f);
    m_model.scale(1.0f/bounds.diagonal());
    m_model.translate(-bounds.center());

    QMatrix4x4 m_view;
    m_view.setToIdentity();
    m_view.lookAt(CAMERA_POSITION, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0});

    float aspect_ratio = float(this->height())/this->width();
    QMatrix4x4 m_projection;
    m_projection.setToIdentity();
    m_projection.ortho(
                (-0.5f*zoom + shift_x),
                ( 0.5f*zoom + shift_x),
                (-0.5f*zoom + shift_y)*aspect_ratio,
                ( 0.5f*zoom + shift_y)*aspect_ratio,
                  0.01f,
                  100.0f
    );

    model_shader->bind();
    model_shader->setUniformValue("modelMatrix", m_model);
    model_shader->setUniformValue("normalMatrix", m_model.normalMatrix());
    model_shader->setUniformValue("viewMatrix", m_view);
    model_shader->setUniformValue("projectionMatrix", m_projection);
    model_shader->release();

    edge_shader->bind();
    edge_shader->setUniformValue("modelMatrix", m_model);
    edge_shader->setUniformValue("viewMatrix", m_view);
    edge_shader->setUniformValue("projectionMatrix", m_projection);
    edge_shader->release();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    background->draw(background_shader);
    limb_faces_right->draw(model_shader);
    limb_edges_right->draw(edge_shader);
    if(symmetry) {
        limb_faces_left->draw(model_shader);
        limb_edges_left->draw(edge_shader);
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
