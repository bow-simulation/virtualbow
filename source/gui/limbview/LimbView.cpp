#include "LimbView.hpp"
#include "LimbMesh.hpp"
#include "MaterialLegend.hpp"
#include "LayerColors.hpp"
#include "OpenGLUtils.hpp"
#include "gui/viewmodel/ViewModel.hpp"
#include "solver/model/input/InputData.hpp"
#include "config.hpp"

#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QCoreApplication>
#include <QToolButton>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QDate>

LimbView::LimbView(ViewModel* model)
    : model(model),
      legend(new MaterialLegend()),
      background_shader(nullptr),
      model_shader(nullptr)
{
    // Anti aliasing
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setSamples(8);
    setFormat(format);

    const QSize BUTTON_SIZE = {34, 34};
    const QString BUTTON_STYLE = "QToolButton { background-color: rgba(220, 220, 220, 200); border-radius: 5px; }"
                                 "QToolButton:pressed { background-color: rgba(150, 150, 150, 200); }"
                                 "QToolButton:checked { background-color: rgba(150, 150, 150, 200); }";

    const QString BUTTON_FRAME_STYLE = "background-color: rgba(38, 38, 38, 100); border-radius: 8px";

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

    auto label = new QLabel(QString("<font color=\"white\" size=\"8\">Virtual<strong>Bow</strong></font><br><font color=\"white\" size=\"4\">Version ") + Config::APPLICATION_VERSION + "</font>");
    label->setAlignment(Qt::AlignRight);

    auto buttonBox = new QHBoxLayout();
    buttonBox->setSpacing(12);
    buttonBox->setContentsMargins(12, 8, 12, 8);
    buttonBox->addWidget(bt_view_3d);
    buttonBox->addWidget(bt_view_profile);
    buttonBox->addWidget(bt_view_top);
    buttonBox->addWidget(bt_view_fit);
    buttonBox->addWidget(bt_view_symmetric);

    auto buttonFrame = new QFrame();
    buttonFrame->setLayout(buttonBox);
    buttonFrame->setStyleSheet(BUTTON_FRAME_STYLE);

    auto row1 = new QHBoxLayout();
    row1->setAlignment(Qt::AlignTop);
    row1->addWidget(legend);
    row1->addStretch();
    row1->addWidget(label);

    auto row2 = new QHBoxLayout();
    row2->setAlignment(Qt::AlignBottom);
    row2->setSpacing(15);
    row2->addStretch();
    row2->addWidget(buttonFrame);
    row2->addStretch();

    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    vbox->setMargin(25);
    vbox->addLayout(row1);
    vbox->addStretch();
    vbox->addLayout(row2);

    // Initialize view parameters
    viewSymmetric(false);
    view3D();

    // Update on changes to the model
    QObject::connect(model, &ViewModel::dimensionsModified, this, &LimbView::updateView);
    QObject::connect(model, &ViewModel::materialsModified, this, &LimbView::updateView);
    QObject::connect(model, &ViewModel::layersModified, this, &LimbView::updateView);
    QObject::connect(model, &ViewModel::profileModified, this, &LimbView::updateView);
    QObject::connect(model, &ViewModel::widthModified, this, &LimbView::updateView);
}

void LimbView::updateView() {
    legend->setData(model->getMaterials());

    LimbMesh mesh(model->getData());
    limb_right = std::make_unique<Model>(mesh.faces_right);
    limb_left = std::make_unique<Model>(mesh.faces_left);

    update();
}

void LimbView::viewProfile() {
    rot_x = 0.0f;
    rot_y = 0.0f;
    viewFit();
}

void LimbView::viewTop() {
    rot_x = 90.0f;
    rot_y = 0.0f;
    viewFit();
}

void LimbView::view3D() {
    rot_x = DEFAULT_ROT_X;
    rot_y = DEFAULT_ROT_Y;
    viewFit();
}

void LimbView::viewSymmetric(bool checked) {
    symmetry = checked;
    update();
}

void LimbView::viewFit() {
    shift_x = 0.0f;
    shift_y = 0.0f;
    zoom = DEFAULT_ZOOM;
    update();
}

void LimbView::initializeGL() {
    initializeOpenGLFunctions();

    // OpenGL configuration

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glLineWidth(1.0f);

    // Shaders

    background_shader = new QOpenGLShaderProgram(this);
    background_shader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/BackgroundShader.vert");
    background_shader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/BackgroundShader.frag");
    background_shader->bindAttributeLocation("modelPosition", 0);
    background_shader->bindAttributeLocation("modelNormal", 1);
    background_shader->bindAttributeLocation("modelColor", 2);
    background_shader->link();

    model_shader = new QOpenGLShaderProgram(this);
    model_shader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/ModelShader.vert");
    model_shader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/ModelShader.frag");
    model_shader->bindAttributeLocation("modelPosition", 0);
    model_shader->bindAttributeLocation("modelNormal", 1);
    model_shader->bindAttributeLocation("modelColor", 2);
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

    // Create background mesh

    Mesh background_mesh(GL_QUADS);
    background_mesh.addVertex({ 1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, BACKGROUND_COLOR_1);
    background_mesh.addVertex({-1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, BACKGROUND_COLOR_1);
    background_mesh.addVertex({-1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, BACKGROUND_COLOR_2);
    background_mesh.addVertex({ 1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, BACKGROUND_COLOR_2);

    QDate date = QDate::currentDate();
    if(date.month() == 12 && (date.day() == 24 || date.day() == 25 || date.day() == 26)) {
        auto create_star = [&](float x0, float y0, float r, float R, float alpha, unsigned n, const QColor& color) {
            float ratio = float(this->width())/float(this->height());
            float beta = 2.0*M_PI/n;
            float z0 = -0.1;
            for(unsigned i = 0; i < n; ++i) {
                float phi = alpha + i*beta;
                QVector3D p0(x0, y0, z0);
                QVector3D p1(x0 - r*sin(phi - beta/2), y0 + ratio*r*cos(phi - beta/2), z0);
                QVector3D p2(x0 - R*sin(phi), y0 + ratio*R*cos(phi), z0);
                QVector3D p3(x0 - r*sin(phi + beta/2), y0 + ratio*r*cos(phi + beta/2), z0);
                background_mesh.addQuad(p0, p1, p2, p3, color);
            }
        };

        srand(std::time(nullptr));
        auto random_in_range = [](float lower, float upper) {
            return lower + static_cast<float>(rand())/static_cast<float>(RAND_MAX/(upper - lower));
        };

        for(unsigned i = 0; i < 30; ++i) {
            float x = random_in_range(-1.0f, 1.0f);
            float y = random_in_range(-1.0f, 1.0f);
            float r = random_in_range(0.001f, 0.005f);
            create_star(x, y, r, 3.0*r, 0.0, 5, BACKGROUND_COLOR_2);
        }
    }

    background = std::make_unique<Model>(background_mesh);
}

void LimbView::paintGL() {
    Bounds bounds = limb_right->getBounds();
    if(symmetry) {
        bounds.extend(limb_left->getBounds());
    }

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

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    background->draw(background_shader);
    limb_right->draw(model_shader);
    if(symmetry) {
        limb_left->draw(model_shader);
    }
}

void LimbView::mousePressEvent(QMouseEvent *event) {
    mouse_pos = event->pos();
}

void LimbView::mouseMoveEvent(QMouseEvent *event) {
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

void LimbView::wheelEvent(QWheelEvent* event) {
    float mouse_ratio_x = float(event->x())/this->width();                 // event->position() is not available on Qt 5.9.5
    float mouse_ratio_y = float(event->y())/this->height();                // event->position() is not available on Qt 5.9.5
    float delta_zoom = -ZOOM_SPEED*event->angleDelta().y()/120.0f*zoom;    // Dividing by 120 gives the number of 15 degree steps on a standard mouse

    shift_x -= (mouse_ratio_x - 0.5f)*delta_zoom;
    shift_y += (mouse_ratio_y - 0.5f)*delta_zoom;
    zoom += delta_zoom;

    update();
}
