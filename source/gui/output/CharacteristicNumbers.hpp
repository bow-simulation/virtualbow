#pragma once
#include <QtWidgets>
#include "bow/input/InputData.hpp"
#include "bow/output/OutputData.hpp"

class NumberGrid : public QWidget
{
public:
    NumberGrid();

    void addColumn();
    void addGroup(const QString& text);
    void addValue(const QString& text, double value);

private:
    QHBoxLayout* columnLayout;
    QVBoxLayout* currentColumn;
    QGridLayout* currentGrid;
};

class StaticNumbers: public NumberGrid
{
public:
    StaticNumbers(const InputData& input, const LimbProperties& limb, const StaticData& statics);
};

class DynamicNumbers: public NumberGrid
{
public:
    DynamicNumbers(const InputData& input, const LimbProperties& limb, const DynamicData& dynamics);
};