#include "LimbSource.hpp"

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

#include <Eigen/Dense>
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

int LimbSource::RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
    // Generate points

    auto points = vtkSmartPointer<vtkPoints>::New();
    points->SetDataType(VTK_DOUBLE);
    //newPoints->Allocate(...);      // Todo: Preallocate

    size_t n_sections = limb.s.size();
    for(size_t i = 0; i < n_sections; ++i)   // Iterate over sections
    {
        // Curve point and normal
        Eigen::Vector3d p{limb.x[i], limb.y[i], 0.0};
        Eigen::Vector3d nw{0.0, 0.0, 0.5*limb.w[i]};
        Eigen::Vector3d nh{-limb.h[i]*std::sin(limb.phi[i]), limb.h[i]*std::cos(limb.phi[i]), 0.0};

        // Cross section vertices
        Eigen::Vector3d p0 = p + nw;
        Eigen::Vector3d p1 = p + nw + nh;
        Eigen::Vector3d p2 = p - nw + nh;
        Eigen::Vector3d p3 = p - nw;

        points->InsertNextPoint(p0.data());
        points->InsertNextPoint(p1.data());
        points->InsertNextPoint(p2.data());
        points->InsertNextPoint(p3.data());
    }

    // Generate indices

    auto polys = vtkSmartPointer<vtkCellArray>::New();
    //newPolys->Allocate(...);        // Todo: Preallocate

    size_t n_segments = (n_sections > 0) ? n_sections-1 : 0;
    for(size_t i = 0; i < n_segments; ++i)
    {
        vtkIdType i0 = 4*i;        // Beginning index of first section
        vtkIdType i1 = i0 + 4;     // Beginning index of second section

        if(i == 0)
        {
            vtkIdType pts_front[] = {i0 + 0, i0 + 1, i0 + 2, i0 + 3};
            polys->InsertNextCell(4, pts_front);
        }
        else if(i == n_segments - 1)
        {
            vtkIdType pts_back[] = {i1 + 0, i1 + 3, i1 + 2, i1 + 1};
            polys->InsertNextCell(4, pts_back);
        }

        vtkIdType pts_top[] = {i0 + 0, i0 + 3, i1 + 3, i1 + 0};
        vtkIdType pts_bottom[] = {i0 + 1, i1 + 1, i1 + 2, i0 + 2};
        vtkIdType pts_left[] = {i0 + 0, i1 + 0, i1 + 1, i0 + 1};
        vtkIdType pts_right[] = {i0 + 2, i1 + 2, i1 + 3, i0 + 3};

        polys->InsertNextCell(4, pts_top);
        polys->InsertNextCell(4, pts_bottom);
        polys->InsertNextCell(4, pts_left);
        polys->InsertNextCell(4, pts_right);
    }

    // Set output stuff

    vtkInformation* info = outputVector->GetInformationObject(0);
    vtkPolyData* output = vtkPolyData::SafeDownCast(info->Get(vtkDataObject::DATA_OBJECT()));
    output->SetPoints(points);
    output->SetPolys(polys);

    return 1;
}

/*
int LimbSource::RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
    // Generate points

    auto points = vtkSmartPointer<vtkPoints>::New();
    points->SetDataType(VTK_DOUBLE);
    //newPoints->Allocate(...);      // Todo: Preallocate

    double XLength = 1.0;
    double YLength = 1.0;
    double ZLength = 1.0;

    Eigen::Vector3d x;
    int i, j, k;

    for(x[0]=-XLength/2.0, i=0; i<2; i++, x[0]+=XLength)
    {
        for(x[1]=-YLength/2.0, j=0; j<2; j++, x[1]+=YLength)
        {
            for(x[2]=-ZLength/2.0, k=0; k<2; k++, x[2]+=ZLength)
            {
                points->InsertNextPoint(x.data());
            }
        }
    }

    for(x[1]=-YLength/2.0, i=0; i<2; i++, x[1]+=YLength)
    {
        for(x[0]=-XLength/2.0, j=0; j<2; j++, x[0]+=XLength)
        {
            for(x[2]=-ZLength/2.0, k=0; k<2; k++, x[2]+=ZLength)
            {
                points->InsertNextPoint(x.data());
            }
        }
    }

    for(x[2]=-ZLength/2.0, i=0; i<2; i++, x[2]+=ZLength)
    {
        for(x[1]=-YLength/2.0, j=0; j<2; j++, x[1]+=YLength)
        {
            for(x[0]=-XLength/2.0, k=0; k<2; k++, x[0]+=XLength)
            {
                points->InsertNextPoint(x.data());
            }
        }
    }


    auto polys = vtkSmartPointer<vtkCellArray>::New();
    //newPolys->Allocate(...);        // Todo: Preallocate

    // Generate indices

    {
        vtkIdType pts[] = {0, 1, 3, 2};
        polys->InsertNextCell(4, pts);
    }

    {
        vtkIdType pts[] = {4, 6, 7, 5};
        polys->InsertNextCell(4, pts);
    }

    {
        vtkIdType pts[] = {8, 10, 11, 9};
        polys->InsertNextCell(4, pts);
    }

    {
        vtkIdType pts[] = {12, 13, 15, 14};
        polys->InsertNextCell(4, pts);
    }

    {
        vtkIdType pts[] = {16, 18, 19, 17};
        polys->InsertNextCell(4, pts);
    }

    {
        vtkIdType pts[] = {20, 21, 23, 22};
        polys->InsertNextCell(4, pts);
    }

    // Set output stuff

    vtkInformation* info = outputVector->GetInformationObject(0);
    vtkPolyData* output = vtkPolyData::SafeDownCast(info->Get(vtkDataObject::DATA_OBJECT()));
    output->SetPoints(points);
    output->SetPolys(polys);

    return 1;
}
*/

