#pragma once
#include <QSpinBox>
#include "extern/calculate/include/calculate.hpp"

class QAbstractItemModel;
class QWheelEvent;
struct IntegerRange;

class IntegerView: public QSpinBox {
    Q_OBJECT

public:
    IntegerView(QAbstractItemModel* model, QPersistentModelIndex index, const IntegerRange& range);

protected:
    virtual void wheelEvent(QWheelEvent *event) override;

private:
    calculate::Parser parser;

    int valueFromText(const QString& text) const override;
    QValidator::State validate(QString &text, int &pos) const override;
};
