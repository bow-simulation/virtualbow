#pragma once
#include "gui/PersistentDialog.hpp"
#include "bow/output/OutputData.hpp"
#include "bow/input/InputData.hpp"
#include <QtWidgets>

class OutputWindow: public QMainWindow
{
public:
    OutputWindow(QWidget* parent, InputData input, OutputData output);
    ~OutputWindow() override;

private:
    InputData input;
    OutputData output;
};

class OutputWidget: public QWidget
{
public:
    OutputWidget(const InputData& input, const OutputData& output);
};

class StaticOutputWidget: public QWidget
{
public:
    StaticOutputWidget(const InputData& input, const LimbProperties& limb, const StaticData& statics);
};

class DynamicOutputWidget: public QWidget
{
public:
    DynamicOutputWidget(const InputData& input, const LimbProperties& limb, const DynamicData& dynamics);
};
