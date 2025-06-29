#pragma once
#include <QSpinBox>
#include "extern/calculate/include/calculate.hpp"

/*
class QAbstractItemModel;
struct DoubleRange;

class DoubleView: public QSpinBox {
    Q_OBJECT

public:
    DoubleView(QAbstractItemModel* model, QPersistentModelIndex index, const Quantity& quantity, const DoubleRange& range);

private:
    static calculate::Parser parser;

    int valueFromText(const QString& text) const override;
    QValidator::State validate(QString &text, int &pos) const override;
};
*/

class QAbstractItemModel;
struct DoubleRange;
class Quantity;

class DoubleView: public QDoubleSpinBox {
    Q_OBJECT

public:
    DoubleView(QAbstractItemModel* model, QPersistentModelIndex index, const Quantity& quantity, const DoubleRange& range);
    void showUnit(bool value);

signals:
    void modified();

protected:
    virtual void wheelEvent(QWheelEvent *event) override;

private:
    bool show_unit;
    const Quantity& quantity;
    calculate::Parser parser;

    QString textFromValue(double value) const override;
    double valueFromText(const QString& text) const override;
    QValidator::State validate(QString &text, int &pos) const override;
    void stepBy(int steps) override;

    void updateUnit();
};

