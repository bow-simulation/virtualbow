#pragma once
#include <QtWidgets>

class Slider: public QWidget
{
    Q_OBJECT

public:
    Slider(const std::vector<double>& values, const QString& text);

signals:
    void valueChanged(int index);
};
