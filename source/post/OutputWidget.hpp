#pragma once
#include "solver/model/output/OutputData.hpp"
#include "gui/units/UnitSystem.hpp"
#include <QtWidgets>

class OutputWidget: public QWidget
{
public:
    OutputWidget(const OutputData& data, const UnitSystem& units);
    ~OutputWidget() override;
    const OutputData& getData();

private:
    OutputData data;
    QPushButton* bt_statics;
    QPushButton* bt_dynamics;
};

class StaticOutputWidget: public QWidget
{
public:
    StaticOutputWidget(const OutputData& data, const UnitSystem& units);
    ~StaticOutputWidget() override;

private:
    QTabWidget* tabs;
};

class DynamicOutputWidget: public QWidget
{
public:
    DynamicOutputWidget(const OutputData& data, const UnitSystem& units);
    ~DynamicOutputWidget() override;

private:
    QTabWidget* tabs;
};
