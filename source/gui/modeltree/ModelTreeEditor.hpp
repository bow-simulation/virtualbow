#pragma once
#include "solver/model/input/InputData.hpp"
#include <QTreeWidget>

class ViewModel;

class ModelTreeEditor: public QTreeWidget
{
    Q_OBJECT

public:
    ModelTreeEditor(ViewModel* model);
    const InputData& getData() const;
    void setData(const InputData& data);

signals:
    void modified();

private:
    InputData data;
};
