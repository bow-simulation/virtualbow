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
#include <vtkFloatArray.h>
#include <vtkCellData.h>

#include "numerics/Eigen.hpp"
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

    Vector<3> p0 = (Vector<3>() << 0.0, 0.0).finished();
    Vector<3> p1 = (Vector<3>() << 1.0, 0.0).finished();
    Vector<3> p2 = (Vector<3>() << 1.0, 0.5).finished();
    Vector<3> p3 = (Vector<3>() << 1.0, 1.0).finished();
    Vector<3> p4 = (Vector<3>() << 0.0, 1.0).finished();
    Vector<3> p5 = (Vector<3>() << 0.0, 0.5).finished();

    points->InsertNextPoint(p0.data());
    points->InsertNextPoint(p1.data());
    points->InsertNextPoint(p2.data());
    points->InsertNextPoint(p3.data());
    points->InsertNextPoint(p4.data());
    points->InsertNextPoint(p5.data());

    // Generate indices

    auto polys = vtkSmartPointer<vtkCellArray>::New();
    auto cellData = vtkSmartPointer<vtkFloatArray>::New();

    //newPolys->Allocate(...);        // Todo: Preallocate

    vtkIdType quad1[] = {0, 1, 2, 5};
    polys->InsertNextCell(4, quad1);
    cellData->InsertNextValue(0);

    vtkIdType quad2[] = {5, 2, 3, 4};
    polys->InsertNextCell(4, quad2);
    cellData->InsertNextValue(1);

    // Set output stuff

    vtkInformation* info = outputVector->GetInformationObject(0);
    vtkPolyData* output = vtkPolyData::SafeDownCast(info->Get(vtkDataObject::DATA_OBJECT()));
    output->SetPoints(points);
    output->SetPolys(polys);
    output->GetCellData()->SetScalars(cellData);

    return 1;
}
