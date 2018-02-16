#pragma once
#include "bow/input/Layers.hpp"
#include <vtkLegendBoxActor.h>

class LayerLegend: public vtkLegendBoxActor
{
public:
    static LayerLegend* New();
    void setData(const Layers& layers);

protected:
    LayerLegend();
};
