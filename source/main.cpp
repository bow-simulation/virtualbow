/*
#include <vtkSmartPointer.h>

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char *[])
{
  // Create a cube.
  vtkSmartPointer<vtkCubeSource> cubeSource =
    vtkSmartPointer<vtkCubeSource>::New();

  // Create a mapper and actor.
  vtkSmartPointer<vtkPolyDataMapper> mapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(cubeSource->GetOutputPort());

  vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  // Create a renderer, render window, and interactor
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();

  renderer->SetViewport(0.0, 0.0, 0.5, 0.5);


  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // Add the actors to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(.3, .2, .1);

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
*/

/*
#include <vtkCubeSource.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindowInteractor.h>

int main(int, char *[])
{
  vtkSmartPointer<vtkCubeSource> source = vtkSmartPointer<vtkCubeSource>::New();

  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(source->GetOutputPort());

  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  actor->SetOrientation(30.0, 30.0, 30.0);

  vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();
  window->SetSize(400, 300);

  vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  interactor->SetRenderWindow(window);

  // Add renderer with specified viewport and camera settings to the window
  auto AddRenderer = [&](bool parallel, bool horizontal, double xmin, double ymin, double xmax, double ymax)
  {
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetViewport(xmin, ymin, xmax, ymax);
    renderer->AddActor(actor);

    vtkCamera* camera = renderer->GetActiveCamera();
    camera->SetParallelProjection(parallel);
    camera->SetUseHorizontalViewAngle(horizontal);
    camera->SetUseHorizontalParallelScale(horizontal);
    renderer->ResetCamera();

    window->AddRenderer(renderer);
  };

  // Top left: perspective projection, vertical view angle
  AddRenderer(false, false, 0.0, 0.5, 0.5, 1.0);

  // Top right: perspective projection, horizontal view angle
  AddRenderer(false,  true, 0.5, 0.5, 1.0, 1.0);

  // Bottom left: parallel projection, vertical parallel scale angle
  AddRenderer( true, false, 0.0, 0.0, 0.5, 0.5);

  // Bottom right: parallel projection, horizontal parallel scale
  AddRenderer( true,  true, 0.5, 0.0, 1.0, 0.5);

  window->Render();
  interactor->Start();

  return EXIT_SUCCESS;
}
*/

#include "gui/MainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    setlocale(LC_NUMERIC, "C");

    QGuiApplication::setApplicationDisplayName("Bow Simulator");
    QGuiApplication::setApplicationVersion("0.1.0");
    QGuiApplication::setOrganizationDomain("https://bow-simulator.sourceforge.net");

    QStringList args = QApplication::arguments();
    MainWindow window(args.size() == 1 ? ":/bows/default.bow" : args.at(1));
    window.show();
    
    return app.exec();
}

/*
#include <iostream>
#include "model/document/DocumentNode.hpp"

int main()
{
    //std::map<std::string, DocumentNode> test = {{"A", Double(3.14)},{"B", Double(3.14)}};

    DocumentNode doc(
    {
        {
            "meta",
            {
                {  "version", Value<std::string>(All<std::string>()) },    // Todo*: Version
                { "comments", Value<std::string>(All<std::string>()) }
            }
        },

        {
            "profile",
            {
                { "arc length", Vector(Positive<std::vector<double>>()) },
                {  "curvature", Vector(All<std::vector<double>>()) },
                {   "offset x", Double(All<double>()) },
                {   "offset y", Double(All<double>()) },
                {      "angle", Double(All<double>()) }
            }
        },

        {
            "width",
            {
                Vector(InclusiveRange<std::vector<double>>(0.0, 1.0) && Ascending<std::vector<double>>()),
                Vector(Positive<std::vector<double>>())
            }
        },

        {
            "layers",
            {
                {
                    { "name", String(All<std::string>()) },
                    {  "rho", Double(Positive<double>()) },
                    {    "E", Double(Positive<double>()) },
                    {
                        "height",
                        {
                            Vector(InclusiveRange<std::vector<double>>(0.0, 100.0) && Ascending<std::vector<double>>()),
                            Vector(Positive<std::vector<double>>())
                        }
                    }
                }
            }
        },

        {
            "string",
            {
                { "strand stiffness", Double(Positive<double>()) },
                {   "strand density", Double(Positive<double>()) },
                {          "strands", Double(Positive<double>()) }
            }
        },

        {
            "masses",
            {
                { "string center", Double(!Negative<double>()) },
                {    "string tip", Double(!Negative<double>()) },
                {      "limb tip", Double(!Negative<double>()) }
            }
        },

        {
            "operation",
            {
                { "brace height", Double(All<double>()) },
                {  "draw length", Double(All<double>()) },
                {   "arrow mass", Double(Positive<double>()) }
            }
        },

        {
            "settings",
            {
                {    "elements limb", Integer(Positive<int>()) },
                {  "elements string", Integer(Positive<int>()) },
                {       "draw steps", Integer(Positive<int>()) },
                { "time span factor", Double(Positive<double>()) },
                { "time step factor", Double(ExclusiveRange<double>(0.0, 1.0)) },
                {    "sampling time", Double(Positive<double>()) }
            }
        }
    });

    return 0;
}
*/
/*
int main()
{
    std::vector<double> a = {0.0, 0.5, 1.0};
    std::cout << (a >= 0.0);

    return 0;
}
*/

/*
    DocItem(value, unit, name);
    double get_base() const;    // Replace with conversion operator to T.
    double get_scaled() const;
    void set_scaled(double value)

    T base_value;
    std::string name;
    Unit unit;
*/

/*
#include <QtWidgets>

class TabBarPlus: public QTabBar
{
public:
    TabBarPlus()
    {
        button = new QPushButton("+", this);
        button->setMaximumSize(20, 20);
        button->setMinimumSize(20, 20);

        setTabsClosable(true);
        movePlusButton();

        QObject::connect(this, &QTabBar::tabBarDoubleClicked, [&](int index)
        {
            qInfo() << "Double click";
        });

        QObject::connect(button, &QPushButton::clicked, [&]()
        {
            qInfo() << "Add tab";
        });
    }

    virtual QSize sizeHint() const override
    {
        // Return the size of the TabBar with increased width for the plus button."""
        QSize sizeHint = QTabBar::sizeHint();
        sizeHint += {25, 0};

        return sizeHint;
    }

    virtual void resizeEvent(QResizeEvent * event) override
    {
        // Resize the widget and make sure the plus button is in the correct location.
        QTabBar::resizeEvent(event);
        movePlusButton();
    }

    virtual void tabLayoutChange()
    {
        // This virtual handler is called whenever the tab layout changes.
        // If anything changes make sure the plus button is in the correct location.
        QTabBar::tabLayoutChange();
        movePlusButton();
    }

    void movePlusButton()
    {
        // Move the plus button to the correct location.
        // Find the width of all of the tabs
        int size = 0;
        for(int i = 0; i < count(); ++i)
            size += tabRect(i).width();

        // Set the plus button location in a visible area
        int h = 0;  // geometry().top();

        button->move(size, h);
    }

private:
    QPushButton* button;
};


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TabBarPlus tb;
    tb.addTab("A");
    tb.addTab("B");
    tb.show();

    return app.exec();
}
*/
