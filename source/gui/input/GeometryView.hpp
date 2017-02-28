#pragma once
#include "../Plot.hpp"
#include "../../model/Document.hpp"
#include "../../model/DiscreteLimb.hpp"

#include <QtWidgets>

class InputData;

class GeometryView: public QLabel
{
public:
    GeometryView(InputData& data)
        : data(data)
    {
        this->setText("Limb geometry!");
    }

private:
    InputData& data;
};
