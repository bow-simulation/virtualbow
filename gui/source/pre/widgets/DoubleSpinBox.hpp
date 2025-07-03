#pragma once
#include <QDoubleSpinBox>

class Quantity;
struct DoubleRange;

class DoubleSpinBox: public QDoubleSpinBox {
public:
    DoubleSpinBox(const Quantity& quantity, const DoubleRange& range, QWidget* parent = nullptr);
    void showUnit(bool value);

signals:
    void modified();

private:
    bool show_unit;
    const Quantity& quantity;

    QString textFromValue(double value) const override;
    double valueFromText(const QString& text) const override;
    QValidator::State validate(QString &text, int &pos) const override;

    void updateUnit();
};
