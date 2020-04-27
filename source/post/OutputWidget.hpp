#pragma once
#include "solver/model/output/OutputData.hpp"
#include <QtWidgets>

class OutputWidget: public QWidget
{
public:
    OutputWidget(const OutputData& output);
    const OutputData& getData();

private:
    OutputData data;
};

class StaticOutputWidget: public QWidget
{
public:
    StaticOutputWidget(const OutputData& output);
    ~StaticOutputWidget();

private:
    QTabWidget* tabs;
};

class DynamicOutputWidget: public QWidget
{
public:
    DynamicOutputWidget(const OutputData& output);
    ~DynamicOutputWidget();

private:
    QTabWidget* tabs;
};
