#pragma once
#include <QWidget>

class Quantity;
class QLineEdit;
class QLabel;
class QSlider;
class QMenu;

class Slider: public QWidget {
    Q_OBJECT

public:
    Slider(const std::vector<double>& values, const QString& text, const Quantity& quantity);
    void addJumpAction(const QString& name, int index);

signals:
    void indexChanged(int index);

private:
    static const int playback_max_fps = 45;
    static const int playback_time = 5000;

    QLineEdit* edit;
    QLabel* label;
    QSlider* slider;
    QMenu* menu;

    const std::vector<double>& values;
    QString text;
    const Quantity& quantity;
    int index;

    void updateLabels();
};
