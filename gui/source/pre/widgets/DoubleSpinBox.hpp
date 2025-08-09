#pragma once
#include "pre/utils/DoubleRange.hpp"
#include <QDoubleSpinBox>

class Quantity;
struct DoubleRange;

class DoubleSpinBox: public QDoubleSpinBox {
    Q_OBJECT

public:
    DoubleSpinBox(const Quantity& quantity, const DoubleRange& range, QWidget* parent = nullptr);
    void showUnit(bool value);

signals:
    void contentModified();

private:
    DoubleRange range;
    bool show_unit;
    const Quantity& quantity;

    QString textFromValue(double baseValue) const override;
    double valueFromText(const QString& text) const override;
    QValidator::State validate(QString &text, int &pos) const override;
    void stepBy(int steps) override;

    void updateUnit();
};
