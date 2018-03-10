#pragma once
#include "bow/input/InputData.hpp"
#include "LimbSource.hpp"
#include "LayerLegend.hpp"
#include <QVTKOpenGLWidget.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkOrientationMarkerWidget.h>

class LimbView: public QVTKOpenGLWidget
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

    virtual QSize sizeHint() const;
    virtual void resizeGL(int w, int h) override;

    void updateIndicatorPosition(const QSize& screen);
    void updateLegendPosition(const QSize& screen);
    void setCameraPosition(double alpha, double beta);
};
