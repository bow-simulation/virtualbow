#pragma once
#include <QDoubleSpinBox>
#include "gui/utils/DoubleRange.hpp"

class UnitGroup;

struct TableSpinnerOptions {
    const UnitGroup& units;
    DoubleRange range;
    double step;

    TableSpinnerOptions(const UnitGroup& units, DoubleRange range, double step)
        : units(units), range(range), step(step) {

    }
};

class TableSpinner: public QDoubleSpinBox {
public:
    TableSpinner(const TableSpinnerOptions& options, QWidget* parent = nullptr);

    QString textFromValue(double value) const override;
    double valueFromText(const QString& text) const override;
};
