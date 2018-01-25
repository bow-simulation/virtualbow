#pragma once
#include "gui/PersistentDialog.hpp"
#include "bow/output/OutputData.hpp"
#include <QtWidgets>

class StaticOutput: public QWidget
{
public:
    StaticOutput(const SetupData& setup, const StaticData& statics);
};

class DynamicOutput: public QWidget
{
public:
    DynamicOutput(const SetupData& setup, const DynamicData& dynamics);
};

class OutputDialog: public PersistentDialog
{
public:
    OutputDialog(QWidget* parent, const OutputData& output);
};
