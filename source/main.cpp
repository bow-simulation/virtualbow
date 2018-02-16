/*
#include <vtkVersion.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyData.h>
#include <vtkCubeSource.h>
#include <vtkSphereSource.h>
#include <vtkLegendBoxActor.h>
#include <vtkNamedColors.h>

#include <vtkSmartPointer.h>

int main (int, char *[])
{
  vtkSmartPointer<vtkSphereSource> sphereSource =
    vtkSmartPointer<vtkSphereSource>::New();
  sphereSource->SetCenter(0.0, 0.0, 0.0);
  sphereSource->SetRadius(5000.0);
  sphereSource->Update();

  vtkPolyData* polydata = sphereSource->GetOutput();

  // Create a mapper
  vtkSmartPointer<vtkPolyDataMapper> mapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(polydata);

  // Create an actor
  vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  // A renderer and render window
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);

  // An interactor
  auto renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  auto legend = vtkSmartPointer<vtkLegendBoxActor>::New();
  legend->SetNumberOfEntries(2);

  auto legendBox = vtkSmartPointer<vtkCubeSource>::New();
  legendBox->Update();

  vtkImageData* symbol = nullptr;
  double color[3] = {1.0, 1.0, 1.0};

  legend->SetEntry(0, symbol, "Layer 0", color);
  legend->SetEntry(1, symbol, "Layer 1", color);

  // place legend in lower right
  legend->GetPositionCoordinate()->SetCoordinateSystemToView();
  legend->GetPosition2Coordinate()->SetCoordinateSystemToView();

  legend->GetPositionCoordinate()->SetValue(.5, -1.0);
  legend->GetPosition2Coordinate()->SetValue(1.0, -0.5);

  //legend->SetBorder(false);

  // Add the actors to the scene
  renderer->AddActor(actor);
  renderer->AddActor(legend);
  renderer->SetBackground(0, 0, 1); // Background color cyan

  // Render an image (lights and cameras are created automatically)
  renderWindow->Render();

  // Begin mouse interaction
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
*/

/*
#include <QtWidgets>

QPixmap drawLayerSymbol(const QColor& color)
{
    // https://stackoverflow.com/a/29196812/4692009
    // https://stackoverflow.com/a/37213313/4692009

    const double size = 48.0;

    QPixmap pixmap(size, size);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    //painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.fillRect(QRectF{0.0, 0.0, size, size}, color);

    //QPainterPath path;
    //path.addRoundedRect(QRectF(0, 0, size, size), 0.3*size, 0.3*size);

    //painter.setPen(Qt::NoPen);
    //painter.fillPath(path, color);
    //painter.drawPath(path);

    return pixmap;
}

class LegendEntry: public QWidget
{
public:
    LegendEntry()
        : symbol(new QLabel()),
          label(new QLabel())
    {
        int size = symbol->sizeHint().height();
        symbol->setFixedWidth(size);
        symbol->setFixedHeight(size);
        symbol->setScaledContents(true);

        auto hbox = new QHBoxLayout();
        hbox->addWidget(symbol, 0);
        hbox->addWidget(label, 1);
        this->setLayout(hbox);
    }

    void setColor(const QColor& color)
    {
        symbol->setPixmap(drawLayerSymbol(color));
    }

    void setText(const QString& text)
    {
        label->setText(text);
    }


private:
    QLabel* symbol;
    QLabel* label;
};


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    LegendEntry entry;
    entry.setColor(QColor{150, 170, 80});
    entry.setText("Layer 123");
    entry.show();

    return app.exec();
}
*/

#include "gui/Application.hpp"

int main(int argc, char* argv[])
{
    return Application::run(argc, argv);
}

/*
#include <QtWidgets>
#include <random>

QColor getColor(double rho, double E)
{
    std::mt19937 rng(rho*E);    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(0, 255); // guaranteed unbiased

    return QColor(uni(rng), uni(rng), uni(rng));
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QLabel label("ThisIsATestWithVeryLongText");
    label.show();

    double E = 1.2e12;
    double rho = 7500;

    QPalette palette = label.palette();
    palette.setColor(label.backgroundRole(), getColor(rho, E));
    label.setPalette(palette);

    return app.exec();
}
*/

/*
#include "gui/EditableTabBar.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    // QApplication::setStyle("windows");

    EditableTabBar tb;
    tb.addTab("Layer0");
    tb.addTab("Layer1");
    tb.addTab("Layer");
    tb.show();

    QObject::connect(&tb, &EditableTabBar::addTabRequested, []()
    {
        qInfo() << "Add Tab!";
    });

    QObject::connect(&tb, &EditableTabBar::tabRenamed, [](int index, const QString& name)
    {
        qInfo() << index << " renamed to " << name;
    });

    return app.exec();
}
*/
