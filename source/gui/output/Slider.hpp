#pragma once
#include <QtWidgets>

// Remove the small arrow from QToolButton, workaround from https://bugreports.qt.io/browse/QTBUG-2036
// Todo: Check back if there is an official solution
class ToolButton : public QToolButton
{
public:
    ToolButton(QWidget* parent = 0): QToolButton(parent)
    {

    }

protected:
    virtual void paintEvent(QPaintEvent*)
    {
        QStylePainter p(this);
        QStyleOptionToolButton opt;
        initStyleOption(&opt);
        opt.features &= (~ QStyleOptionToolButton::HasMenu);
        p.drawComplexControl(QStyle::CC_ToolButton, opt);
    }
};

class Slider: public QWidget
{
    Q_OBJECT

public:
    Slider(const std::vector<double>& values, const QString& text);

signals:
    void valueChanged(int index);
};
