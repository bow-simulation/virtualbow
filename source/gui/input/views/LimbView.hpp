#pragma once
#include "LimbSource.hpp"
#include "LayerLegend.hpp"
#include "LayerColors.hpp"
#include "bow/LimbProperties.hpp"
#include "bow/input/InputData.hpp"
#include <QtWidgets>

#include <QVTKWidget.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkLookupTable.h>

// Todo
// - Edge tracing
// - Coordinate axes
// - ...

class InputData;

class LimbView: public QVTKWidget
{
public:
    LimbView(): legend(new LayerLegend())
    {
        source = vtkSmartPointer<LimbSource>::New();
        colors = vtkSmartPointer<vtkLookupTable>::New();

        auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(source->GetOutputPort());
        mapper->SetUseLookupTableScalarRange(true);
        mapper->SetLookupTable(colors);

        // Right limb
        actor_r = vtkSmartPointer<vtkActor>::New();
        // Todo: Why is the face orientation wrong inside QVTKWidget? (http://stackoverflow.com/questions/24131430/vtk-6-1-and-qt-5-3-3d-objects-in-qvtkwidget-with-bad-transparencies)
        actor_r->GetProperty()->SetFrontfaceCulling(false);
        actor_r->GetProperty()->SetBackfaceCulling(true);
        actor_r->SetMapper(mapper);

        // Left limb
        actor_l = vtkSmartPointer<vtkActor>::New();
        // Todo: Why is the face orientation wrong inside QVTKWidget? (http://stackoverflow.com/questions/24131430/vtk-6-1-and-qt-5-3-3d-objects-in-qvtkwidget-with-bad-transparencies)
        actor_l->GetProperty()->SetFrontfaceCulling(false);
        actor_l->GetProperty()->SetBackfaceCulling(true);
        actor_l->SetMapper(mapper);
        actor_l->SetOrientation(0.0, 180.0, 0.0);
        actor_l->SetVisibility(false);

        renderer = vtkSmartPointer<vtkRenderer>::New();
        this->GetRenderWindow()->AddRenderer(renderer);
        renderer->AddActor(actor_l);
        renderer->AddActor(actor_r);
        renderer->SetBackground(0.2, 0.3, 0.4);
        renderer->SetUseFXAA(true);    // Todo: What if this is not supported (or can that be reasonably assumed?)

        // Integration of vtkOrientationMarkerWidget and QVTKWidget
        // http://vtk.markmail.org/message/cgkqlbz3jgmn6h3z?q=vtkOrientationMarkerWidget+qvtkwidget
        widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
        widget->SetInteractor(this->GetInteractor());
        widget->SetDefaultRenderer(renderer);
        widget->SetOrientationMarker(vtkSmartPointer<vtkAxesActor>::New());
        widget->SetEnabled(true);
        widget->SetInteractive(false);

        auto camera = renderer->GetActiveCamera();
        camera->SetParallelProjection(true);
        camera->SetUseHorizontalParallelScale(true);

        // Buttons

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
        // hbox->addWidget(legend);
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

    void setData(const InputData& data)
    {
        try
        {
            // Geometry
            source->SetLimbData(LimbProperties(data, 150));    // Magic number

            // Colors
            colors->SetNumberOfColors(data.layers.size());
            colors->SetTableRange(0, data.layers.size());
            colors->Build();
            for(int i = 0; i < data.layers.size(); ++i)
            {
                QColor color = getLayerColor(data.layers[i]);
                colors->SetTableValue(i, color.redF(), color.greenF(), color.blueF(), color.alphaF());
            }

            // Legend
            legend->setData(data.layers);

            // Update
            this->GetInteractor()->Render();
        }
        catch(std::runtime_error& e)
        {
            // Input data invalid, do nothing. Leave geometry in previous state until the input is valid again.
        }
    }

    void viewProfile()
    {
        setCameraPosition(-M_PI_2, 0.0);
        viewFit();
    }

    void viewTop()
    {
        setCameraPosition(-M_PI_2, M_PI_2);
        viewFit();
    }

    void view3D()
    {
        setCameraPosition(-0.9, 0.5);
        viewFit();
    }

    void viewSymmetric(bool checked)
    {
        actor_l->SetVisibility(checked);
        this->GetInteractor()->Render();
    }

    void viewFit()
    {
        renderer->ResetCamera();
        renderer->GetActiveCamera()->Zoom(0.98);    // Magic number
        this->GetInteractor()->Render();    // http://vtk.markmail.org/message/nyq3dwlyfrivrqac
    }

private:
    LayerLegend* legend;
    vtkSmartPointer<LimbSource> source;
    vtkSmartPointer<vtkLookupTable> colors;
    vtkSmartPointer<vtkActor> actor_r;
    vtkSmartPointer<vtkActor> actor_l;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkOrientationMarkerWidget> widget;

    // Adjust orientation widget's viewport on resize to keep it at a constant screen size
    virtual void resizeEvent(QResizeEvent* event) override
    {
        const int size = 200;   // Magic number
        widget->SetViewport(0.0, 0.0, double(size)/event->size().width(), double(size)/event->size().height());
    }

    // alpha: azimuth, beta: elevation.
    // Camera position: Ry(alpha)*Rz(beta)*[1, 0, 0].
    // Camera view up : Ry(alpha)*Rz(beta)*[0, 1, 0].
    void setCameraPosition(double alpha, double beta)
    {
        auto camera = renderer->GetActiveCamera();
        camera->SetFocalPoint(0.0, 0.0, 0.0);
        camera->SetPosition(cos(alpha)*cos(beta), sin(beta), -sin(alpha)*cos(beta));
        camera->SetViewUp(-cos(alpha)*sin(beta), cos(beta), sin(alpha)*sin(beta));
    }
};
