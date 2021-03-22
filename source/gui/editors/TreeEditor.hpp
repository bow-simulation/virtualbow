#pragma once
#include "solver/model//input/InputData.hpp"
#include "gui/units/UnitSystem.hpp"
#include <QTreeWidget>

class TreeEditor: public QTreeWidget
{
    Q_OBJECT

public:
    TreeEditor(const UnitSystem& units);
    const InputData& getData() const;
    void setData(const InputData& data);

signals:
    void modified();

private:
    InputData data;
};
