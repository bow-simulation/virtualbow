#pragma once
#include "solver/BowResult.hpp"
#include <QWidget>

class QPushButton;
class QTabWidget;

class OutputWidget: public QWidget {
public:
    OutputWidget(const BowResult& data);
    ~OutputWidget() override;
    const BowResult& getData();

private:
    BowResult data;
    QPushButton* button_statics;
    QPushButton* button_dynamics;
};

class StaticOutputWidget: public QWidget {
public:
    StaticOutputWidget(const BowResult& data);
    ~StaticOutputWidget() override;

private:
    QTabWidget* tabs;
};

class DynamicOutputWidget: public QWidget {
public:
    DynamicOutputWidget(const BowResult& data);
    ~DynamicOutputWidget() override;

private:
    QTabWidget* tabs;
};
