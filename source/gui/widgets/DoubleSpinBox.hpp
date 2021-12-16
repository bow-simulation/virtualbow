#pragma once
#include <QDoubleSpinBox>

class UnitGroup;

enum class Domain {
    UNRESTRICTED,
    POSITIVE,
    NEGATIVE,
    NON_POSITIVE,
    NON_NEGATIVE
};

class DoubleSpinBox: public QDoubleSpinBox {
public:
    DoubleSpinBox(Domain range, QWidget* parent = nullptr);

    void setUnitGroup(const UnitGroup* new_units);

    QString textFromValue(double value) const override;
    double valueFromText(const QString& text) const override;

private:
    const UnitGroup* units;
    void updateUnit();

signals:
    void modified();
};
