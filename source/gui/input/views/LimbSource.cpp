#include "LimbSource.hpp"
#include "numerics/Eigen.hpp"

#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkCellData.h>

#include <array>

vtkStandardNewMacro(LimbSource)

LimbSource::LimbSource()
{
    this->SetNumberOfInputPorts(0);
}

void LimbSource::SetLimbData(const LimbProperties& limb)
{
    this->limb = limb;
    this->Modified();
}

// Example for mesh with different face colors:
// https://www.vtk.org/Wiki/VTK/Examples/PolyData/ColorCells

int LimbSource::RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
    // Points, polygond and cell data (contains color index)
    auto points = vtkSmartPointer<vtkPoints>::New();
    auto polys = vtkSmartPointer<vtkCellArray>::New();
    auto data = vtkSmartPointer<vtkFloatArray>::New();

    //points->Allocate(...);    // Todo: Preallocate
    //polys->Allocate(...);    // Todo: Preallocate
    //data->Allocate(...);    // Todo: Preallocate

    size_t n_sections = limb.length.size();
    size_t n_layers = limb.layers.size();

    std::vector<int> indices_l0(n_layers + 1);
    std::vector<int> indices_r0(n_layers + 1);
    std::vector<int> indices_l1(n_layers + 1);
    std::vector<int> indices_r1(n_layers + 1);

    for(size_t i = 0; i < n_sections; ++i)
    {
        indices_l0 = indices_l1;
        indices_r0 = indices_r1;

        for(size_t j = 0; j < n_layers; ++j)
        {
            // Curve point (center) and normals in width and height direction
            Vector<3> center  { limb.x_pos[i], limb.y_pos[i], 0.0 };
            Vector<3> normal_w{ 0.0, 0.0, 1.0 };
            Vector<3> normal_h{-sin(limb.angle[i]), cos(limb.angle[i]), 0.0 };

            Vector<3> pl = center - normal_w*limb.width[i] + normal_h*limb.layers[j].y_back[i];
            indices_l1[j] = points->InsertNextPoint(pl.data());

            Vector<3> pr = center + normal_w*limb.width[i] + normal_h*limb.layers[j].y_back[i];
            indices_r1[j] = points->InsertNextPoint(pr.data());

            if(j == n_layers-1)
            {
                Vector<3> pl = center - normal_w*limb.width[i] + normal_h*limb.layers[j].y_belly[i];
                indices_l1[j+1] = points->InsertNextPoint(pl.data());

                Vector<3> pr = center + normal_w*limb.width[i] + normal_h*limb.layers[j].y_belly[i];
                indices_r1[j+1] = points->InsertNextPoint(pr.data());
            }
        }

        // Limb root
        if(i == 0)
        {
            for(size_t j = 0; j < n_layers; ++j)
            {
                vtkIdType quad[] = {indices_r1[j], indices_l1[j], indices_l1[j+1], indices_r1[j+1]};
                polys->InsertNextCell(4, quad);
                data->InsertNextValue(j);
            }
        }

        // Limb tip
        if(i == n_sections-1)
        {
            for(size_t j = 0; j < n_layers; ++j)
            {
                vtkIdType quad[] = {indices_l1[j], indices_r1[j], indices_r1[j+1], indices_l1[j+1]};
                polys->InsertNextCell(4, quad);
                data->InsertNextValue(j);
            }
        }

        // Intermediate sections
        if(i > 0)
        {
            for(size_t j = 0; j < n_layers; ++j)
            {
                // Left side
                vtkIdType quad_l[] = {indices_l0[j], indices_l1[j], indices_l1[j+1], indices_l0[j+1]};
                polys->InsertNextCell(4, quad_l);
                data->InsertNextValue(j);

                // Right side
                vtkIdType quad_r[] = {indices_r0[j], indices_r0[j+1], indices_r1[j+1], indices_r1[j]};
                polys->InsertNextCell(4, quad_r);
                data->InsertNextValue(j);

                // Bottom
                if(j == 0)
                {
                    vtkIdType quad[] = {indices_l0[j], indices_r0[j], indices_r1[j], indices_l1[j]};
                    polys->InsertNextCell(4, quad);
                    data->InsertNextValue(j);
                }

                // Top
                if(j == n_layers-1)
                {
                    vtkIdType quad[] = {indices_l0[j+1], indices_l1[j+1], indices_r1[j+1], indices_r0[j+1]};
                    polys->InsertNextCell(4, quad);
                    data->InsertNextValue(j);
                }
            }
        }
    }

    // Set output stuff

    vtkInformation* info = outputVector->GetInformationObject(0);
    vtkPolyData* output = vtkPolyData::SafeDownCast(info->Get(vtkDataObject::DATA_OBJECT()));
    output->SetPoints(points);
    output->SetPolys(polys);
    output->GetCellData()->SetScalars(data);

    return 1;
}
