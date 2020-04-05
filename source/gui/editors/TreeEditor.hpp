#pragma once
#include "solver/model//input/InputData.hpp"
#include <QtWidgets>

class TreeEditor: public QTreeWidget
{
    Q_OBJECT

public:
    TreeEditor();
    const InputData& getData() const;
    void setData(const InputData& data);

signals:
    void modified();

private:
    InputData data;
};
