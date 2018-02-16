#pragma once
#include "bow/input/Layers.hpp"
#include "LayerColors.hpp"
#include <vtkLegendBoxActor.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkQImageToImageSource.h>
#include <vtkProperty2D.h>

// https://www.vtk.org/Wiki/VTK/Examples/Cxx/Visualization/Legend
// https://www.vtk.org/Wiki/VTK/Examples/Cxx/Qt/QImageToImageSource

class LayerLegend: public vtkLegendBoxActor
{
public:
    static LayerLegend* New();

    void setData(const Layers& layers)
    {
        this->SetNumberOfEntries(layers.size());
        for(size_t i = 0; i < layers.size(); ++i)
        {
            QImage image = getLayerPixmap(layers[i]).toImage();
            auto source = vtkSmartPointer<vtkQImageToImageSource>::New();
            source->SetQImage(&image);
            source->Update();

            double color[3] = {1.0, 1.0, 1.0};
            this->SetEntry(i, source->GetOutput(), layers[i].name.c_str(), color);
        }
    }

protected:
    LayerLegend()
    {
        this->SetBorder(false);
        this->SetBox(true);    // For layout reasons
        this->GetBoxProperty()->SetOpacity(0.0);
    }
};
