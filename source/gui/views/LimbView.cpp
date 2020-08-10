#include "LimbView.hpp"
#include "LayerColors.hpp"

#include <QMouseEvent>
#include <QCoreApplication>

static const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "out vec3 ourColor;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos, 1.0);\n"
        "   ourColor = aColor;\n"
        "}\0";

static const char* fragmentShaderSource =
        "#version 330 core\n"
        "in vec3 ourColor;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = vec4(ourColor, 1.0f);\n"
        "}\0";

// Adapted from https://learnopengl.com/Model-Loading/Mesh
struct Vertex {
    QVector3D position;
    QVector3D color;
};

/*
// Adapted from https://learnopengl.com/Model-Loading/Mesh
class Mesh: public QOpenGLExtraFunctions
{
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices)
        : vertices(vertices), indices(indices)
    {
        initializeOpenGLFunctions();

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                     &indices[0], GL_STATIC_DRAW);

        // Vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));

        // Vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));

        // Vertex colors
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, color));

        glBindVertexArray(0);
    }

    void draw(QOpenGLShaderProgram* shader)
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
    unsigned int VAO, VBO, EBO;
};
*/

class BackgroundMesh
{
public:
    BackgroundMesh(const QColor& color1, const QColor& color2) {
        unsigned i0 = addVertex({ 1.0f, -1.0f, 0.0f}, color1);
        unsigned i1 = addVertex({-1.0f, -1.0f, 0.0f}, color1);
        unsigned i2 = addVertex({-1.0f,  1.0f, 0.0f}, color2);
        unsigned i3 = addVertex({ 1.0f,  1.0f, 0.0f}, color2);

        addTriangle(i0, i1, i2);
        addTriangle(i2, i3, i0);
    }

    const std::vector<Vertex>& getVertices() const {
        return vertices;
    }

    const std::vector<unsigned>& getIndices() const {
        return indices;
    }

private:
    // Add triangle between three indices
    void addTriangle(unsigned i0, unsigned i1, unsigned i2) {
        indices.push_back(i0);
        indices.push_back(i1);
        indices.push_back(i2);
    }

    // Add vertex and return index
    unsigned addVertex(const QVector3D& position, const QColor& color) {
        vertices.push_back({
            .position = position,
            .color = QVector3D(color.redF(), color.greenF(), color.blueF())
        });

        return vertices.size() - 1;
    }

    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
};

LimbView::LimbView()
    : legend(new LayerLegend()),
      limb_mesh_left(true),
      limb_mesh_right(false),
      shader_program(nullptr)
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

    shader_program = new QOpenGLShaderProgram(this);
    shader_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    shader_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    shader_program->link();

    //BackgroundMesh background(BACKGROUND_COLOR_1, BACKGROUND_COLOR_2);

    glGenVertexArrays(1, &VAO);
    unsigned int VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    BackgroundMesh background(BACKGROUND_COLOR_1, BACKGROUND_COLOR_2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, background.getVertices().size()*sizeof(Vertex), background.getVertices().data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, background.getIndices().size()*sizeof(unsigned), background.getIndices().data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 
}

void LimbView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(VAO);

    shader_program->bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
