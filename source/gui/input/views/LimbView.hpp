#pragma once

#include "vtkAutoInit.h"
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingFreeType)

#include "bow/input/InputData.hpp"
#include "LimbSource.hpp"
#include "LayerLegend.hpp"
#include <QVTKWidget.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkOrientationMarkerWidget.h>

class LimbView: public QVTKWidget
{
public:
    LimbView();
    void setData(const InputData& data);

    void viewProfile();
    void viewTop();
    void view3D();
    void viewSymmetric(bool checked);
    void viewFit();

private:
    vtkSmartPointer<LimbSource> source;
    vtkSmartPointer<vtkLookupTable> colors;
    vtkSmartPointer<vtkActor> actor_r;
    vtkSmartPointer<vtkActor> actor_l;
    vtkSmartPointer<LayerLegend> legend;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkOrientationMarkerWidget> indicator;

    virtual void resizeEvent(QResizeEvent* event) override;

    void updateIndicatorPosition(const QSize& screen);
    void updateLegendPosition(const QSize& screen);
    void setCameraPosition(double alpha, double beta);
};
