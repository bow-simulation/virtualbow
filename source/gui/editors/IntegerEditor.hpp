#pragma once
#include <QtWidgets>

class IntegerEditor: public QWidget
{
    Q_OBJECT

public:
    IntegerEditor(const QString& text);

    void setData(int value);
    double getData() const;

signals:
    void modified();

private:
    QLabel* label;
    QSpinBox* edit;
};
