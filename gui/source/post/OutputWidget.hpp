#pragma once
#include "solver/model/output/OutputData.hpp"
#include "solver/model/output2/OutputData2.hpp"
#include <QWidget>

class QPushButton;
class QTabWidget;

class OutputWidget: public QWidget {
public:
    OutputWidget(const OutputData2& data);
    ~OutputWidget() override;
    const OutputData2& getData();

private:
    OutputData2 data;
    QPushButton* button_statics;
    QPushButton* button_dynamics;
};

class StaticOutputWidget: public QWidget {
public:
    StaticOutputWidget(const OutputData2& data);
    ~StaticOutputWidget() override;

private:
    QTabWidget* tabs;
};

class DynamicOutputWidget: public QWidget {
public:
    DynamicOutputWidget(const OutputData2& data);
    ~DynamicOutputWidget() override;

private:
    QTabWidget* tabs;
};
