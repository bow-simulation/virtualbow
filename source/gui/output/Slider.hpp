#pragma once
#include <QtWidgets>

// Remove the small arrow from QToolButton, workaround from https://bugreports.qt.io/browse/QTBUG-2036
// Todo: Check back if there is an official solution
class ToolButton: public QToolButton
{
public:
    ToolButton(QWidget* parent = 0);

protected:
    virtual void paintEvent(QPaintEvent*);
};

class Slider: public QWidget
{
    Q_OBJECT

public:
    Slider(const std::vector<double>& values, const QString& text);

signals:
    void valueChanged(int index);
};
