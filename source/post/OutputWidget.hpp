#pragma once
#include "solver/model/output/OutputData.hpp"
#include "gui/units/UnitSystem.hpp"
#include <QtWidgets>

class OutputWidget: public QWidget {
public:
    OutputWidget(const OutputData& data);
    ~OutputWidget() override;
    const OutputData& getData();

private:
    OutputData data;
    QPushButton* button_statics;
    QPushButton* button_dynamics;
};

class StaticOutputWidget: public QWidget {
public:
    StaticOutputWidget(const OutputData& data);
    ~StaticOutputWidget() override;

private:
    QTabWidget* tabs;
};

class DynamicOutputWidget: public QWidget {
public:
    DynamicOutputWidget(const OutputData& data);
    ~DynamicOutputWidget() override;

private:
    QTabWidget* tabs;
};
