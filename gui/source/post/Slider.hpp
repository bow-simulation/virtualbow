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
    static const int PLAYBACK_MAX_FPS = 30;        // Maximum number of frames per second (skip simulation states if below)
    static const int PLAYBACK_PERIOD_MS = 5000;    // Time period of a single playback from start to end (controls time scaling)

    QLineEdit* edit;
    QLabel* label;
    QSlider* slider;
    QMenu* menu;

    const std::vector<double>& values;
    QString text;
    const Quantity& quantity;

    void updateLabels();
};
