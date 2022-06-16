#pragma once
#include <QDoubleSpinBox>

class UnitGroup;
class DoubleRange;

class DoubleSpinBox: public QDoubleSpinBox {
public:
    DoubleSpinBox(const UnitGroup& units, const DoubleRange& range, QWidget* parent = nullptr);

    QString textFromValue(double value) const override;
    double valueFromText(const QString& text) const override;

private:
    const UnitGroup& units;
    void updateUnit();

signals:
    void modified();
};
