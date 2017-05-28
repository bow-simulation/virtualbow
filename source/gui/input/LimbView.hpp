#pragma once
#include "model/Document.hpp"
#include "model/LimbProperties.hpp"

#include <QtWidgets>

class InputData;

class LimbView: public QWidget
{
public:
    LimbView(InputData& data)
        : data(data)
    {

    }

private:
    InputData& data;
};
