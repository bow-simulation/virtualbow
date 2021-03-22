#pragma once
#include <QWidget>
#include <QLabel>
#include <QSpinBox>

class IntegerEditor: public QWidget {
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
