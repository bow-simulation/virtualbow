#pragma once
#include <QDoubleSpinBox>

class UnitGroup;
class DoubleRange;

class DoubleSpinBox: public QDoubleSpinBox {
    Q_OBJECT

public:
    DoubleSpinBox(const UnitGroup& units, const DoubleRange& range, QWidget* parent = nullptr);

signals:
    void stepped();

private:
    const UnitGroup& units;

    QString textFromValue(double value) const override;
    double valueFromText(const QString& text) const override;
    QValidator::State validate(QString &text, int &pos) const override;
    void stepBy(int steps) override;

    void updateUnit();
};
