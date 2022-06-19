#pragma once
#include <QDoubleSpinBox>
#include "gui/widgets/calculate/include/calculate.hpp"

class IntegerRange;

class IntegerSpinBox: public QSpinBox {
    Q_OBJECT

public:
    IntegerSpinBox(const IntegerRange& range, QWidget* parent = nullptr);

signals:
    void modified();

private:
    static calculate::Parser parser;

    int valueFromText(const QString& text) const override;
    QValidator::State validate(QString &text, int &pos) const override;
    void stepBy(int steps) override;
};
