#pragma once
#include "gui/PersistentDialog.hpp"
#include "bow/output/OutputData.hpp"
#include "bow/input/InputData.hpp"
#include <QtWidgets>

class StaticOutput: public QWidget
{
public:
    StaticOutput(const InputData& input, const LimbProperties& limb, const StaticData& statics);
};

class DynamicOutput: public QWidget
{
public:
    DynamicOutput(const InputData& input, const LimbProperties& limb, const DynamicData& dynamics);
};

class OutputDialog: public PersistentDialog
{
public:
    OutputDialog(QWidget* parent, const InputData& input, const OutputData& output);
};
