#pragma once
#include <vtkPolyDataAlgorithm.h>
#include "bow/LimbProperties.hpp"

// Adapted from vtkCubeSource.h
class LimbSource: public vtkPolyDataAlgorithm
{
public:
    static LimbSource* New();
    void SetLimbData(const LimbProperties& limb);

protected:
    LimbSource();
    ~LimbSource() override {}

    int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

private:
    LimbProperties limb;
};

