#pragma once
#include <QObject>
#include "solver/model/input/InputData.hpp"

class ViewModel: public QObject
{
    Q_OBJECT

public:
    const InputData& getData() const {
        return data;
    }

private:
    InputData data;
};
