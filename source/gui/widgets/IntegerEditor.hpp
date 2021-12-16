#pragma once
#include <QWidget>

class QLabel;
class QSpinBox;

class IntegerEditor: public QWidget {
    Q_OBJECT

public:
    IntegerEditor(const QString& text);

    void setData(int value);
    int getData() const;

signals:
    void modified();

private:
    QLabel* label;
    QSpinBox* edit;
};
