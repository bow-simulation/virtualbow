#pragma once
#include <QtWidgets>

class DoubleEditor: public QWidget
{
    Q_OBJECT

public:
    DoubleEditor(const QString& text);

    void setData(double data);
    double getData() const;

signals:
    void modified();

private:
    QLabel* label;
    QLineEdit* edit;
    bool changed;
};
