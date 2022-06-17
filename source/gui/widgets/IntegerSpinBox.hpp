#pragma once
#include <QDoubleSpinBox>

class IntegerRange;

class IntegerSpinBox: public QSpinBox {
    Q_OBJECT

public:
    IntegerSpinBox(const IntegerRange& range, QWidget* parent = nullptr);

signals:
    void stepped();

private:
    int valueFromText(const QString& text) const override;
    QValidator::State validate(QString &text, int &pos) const override;
    void stepBy(int steps) override;
};
