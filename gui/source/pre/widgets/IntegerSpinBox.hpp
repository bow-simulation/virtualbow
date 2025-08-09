#pragma once
#include <QSpinBox>

struct IntegerRange;

class IntegerSpinBox: public QSpinBox {
public:
    IntegerSpinBox(const IntegerRange& range, QWidget* parent = nullptr);

private:
    int valueFromText(const QString& text) const override;
    QValidator::State validate(QString &text, int &pos) const override;
};
