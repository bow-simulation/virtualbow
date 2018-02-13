/*
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkLookupTable.h>
#include <vtkFloatArray.h>
#include <vtkCellData.h>
#include <vtkPolyData.h>
#include <vtkPlaneSource.h>

#include <algorithm>

int main(int , char *[])
{
  // Provide some geometry
  int resolution = 3;
  vtkSmartPointer<vtkPlaneSource> aPlane =
    vtkSmartPointer<vtkPlaneSource>::New();
  aPlane->SetXResolution(resolution);
  aPlane->SetYResolution(resolution);

  // Create cell data
  vtkSmartPointer<vtkFloatArray> cellData =
    vtkSmartPointer<vtkFloatArray>::New();
  for (int i = 0; i < resolution * resolution; i++)
    {
    cellData->InsertNextValue(i + 1);
    }

  // Create a lookup table to map cell data to colors
  vtkSmartPointer<vtkLookupTable> lut =
    vtkSmartPointer<vtkLookupTable>::New();
  int tableSize = std::max(resolution*resolution + 1, 10);
  lut->SetNumberOfTableValues(tableSize);
  lut->Build();

  // Fill in a few known colors, the rest will be generated if needed
  lut->SetTableValue(0     , 0     , 0     , 0, 1);  //Black
  lut->SetTableValue(1, 0.8900, 0.8100, 0.3400, 1); // Banana
  lut->SetTableValue(2, 1.0000, 0.3882, 0.2784, 1); // Tomato
  lut->SetTableValue(3, 0.9608, 0.8706, 0.7020, 1); // Wheat
  lut->SetTableValue(4, 0.9020, 0.9020, 0.9804, 1); // Lavender
  lut->SetTableValue(5, 1.0000, 0.4900, 0.2500, 1); // Flesh
  lut->SetTableValue(6, 0.5300, 0.1500, 0.3400, 1); // Raspberry
  lut->SetTableValue(7, 0.9804, 0.5020, 0.4471, 1); // Salmon
  lut->SetTableValue(8, 0.7400, 0.9900, 0.7900, 1); // Mint
  lut->SetTableValue(9, 0.2000, 0.6300, 0.7900, 1); // Peacock

  aPlane->Update(); // Force an update so we can set cell data
  aPlane->GetOutput()->GetCellData()->SetScalars(cellData);

  // Setup actor and mapper
  vtkSmartPointer<vtkPolyDataMapper> mapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(aPlane->GetOutputPort());
  mapper->SetScalarRange(0, tableSize - 1);
  mapper->SetLookupTable(lut);

  vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  // Setup render window, renderer, and interactor
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderer->AddActor(actor);
  renderer->SetBackground(.1,.2,.3);
  renderWindow->Render();
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
