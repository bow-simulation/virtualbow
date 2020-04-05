#pragma once
#include <QtWidgets>

class Slider: public QWidget
{
    Q_OBJECT

public:
    Slider(const std::vector<double>& values, const QString& text);
    void addJumpAction(const QString& name, int index);

private:
    static const int playback_max_fps = 45;
    static const int playback_time = 5000;

    QSlider* slider;
    QMenu* menu;

signals:
    void valueChanged(int index);
};
