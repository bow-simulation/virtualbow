#pragma once
#include <QSpinBox>
#include "extern/calculate/include/calculate.hpp"

struct IntegerRange;

class IntegerSpinBox: public QSpinBox {
    Q_OBJECT

public:
    IntegerSpinBox(const IntegerRange& range, QWidget* parent = nullptr);

signals:
    void modified();

private:
    calculate::Parser parser;

    int valueFromText(const QString& text) const override;
    QValidator::State validate(QString &text, int &pos) const override;
    void stepBy(int steps) override;
};
