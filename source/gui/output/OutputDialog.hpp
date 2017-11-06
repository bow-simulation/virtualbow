#pragma once
#include "model/output/OutputData.hpp"
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

class OutputDialog: public QDialog
{
public:
    OutputDialog(QWidget* parent, const OutputData& output);
};
