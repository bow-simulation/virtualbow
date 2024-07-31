#include "Slider.hpp"
#include "pre/viewmodel/units/Quantity.hpp"
#include <QLineEdit>
#include <QDoubleValidator>
#include <QToolButton>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QMenu>
#include <QTimer>
#include <cmath>

Slider::Slider(const std::vector<double>& values, const QString& text, const Quantity& quantity)
    : edit(new QLineEdit()),
      label(new QLabel()),
      slider(new QSlider(Qt::Horizontal)),
      menu(new QMenu()),
      values(values),
      text(text),
      quantity(quantity),
      index(0)
{
    const int height = 30; // Magic number

    edit = new QLineEdit();
    edit->setFixedHeight(height);
    edit->setValidator(new QDoubleValidator(values.front(), values.back(), 10));

    auto button_jump_to = new QToolButton();
    button_jump_to->setIcon(QIcon(":/icons/media-jump-to.svg"));
    button_jump_to->setFixedSize(height, height);
    button_jump_to->setStyleSheet("QToolButton::menu-indicator { image: none; }");
    button_jump_to->setMenu(menu);
    button_jump_to->setPopupMode(QToolButton::InstantPopup);

    auto button_skip_backward = new QToolButton();
    button_skip_backward->setIcon(QIcon(":/icons/media-skip-backward.svg"));
    button_skip_backward->setFixedSize(height, height);

    auto button_play_pause = new QToolButton();
    button_play_pause->setIcon(QIcon(":/icons/media-playback-start.svg"));
    button_play_pause->setFixedSize(height, height);

    auto button_skip_forward = new QToolButton();
    button_skip_forward->setIcon(QIcon(":/icons/media-skip-forward.svg"));
    button_skip_forward->setFixedSize(height, height);

    auto hbox = new QHBoxLayout();
    this->setLayout(hbox);
    hbox->setSpacing(0);
    hbox->addWidget(label);
    hbox->addSpacing(5);
    hbox->addWidget(edit);
    hbox->addWidget(button_jump_to);
    hbox->addSpacing(10);
    hbox->addWidget(button_skip_backward);
    hbox->addSpacing(2);
    hbox->addWidget(button_play_pause);
    hbox->addSpacing(2);
    hbox->addWidget(button_skip_forward);
    hbox->addSpacing(10);
    hbox->addWidget(slider, 1);

    // Timer interval and number of steps done by the timer each tick
    int delta_i = std::ceil(1000*double(values.size() - 1)/(playback_time*playback_max_fps));
    int delta_t = playback_time*delta_i/(values.size() - 1);
    slider->setRange(0, values.size()-1);

    auto timer = new QTimer(this);
    timer->setInterval(delta_t);

    auto start_playback = [=] {
        timer->start();
        button_play_pause->setIcon(QIcon(":/icons/media-playback-pause.svg"));
    };

    auto stop_playback = [=] {
        timer->stop();
        button_play_pause->setIcon(QIcon(":/icons/media-playback-start.svg"));
    };

    // Timer: Advance slider by delta_i if possible, else skip the last bit and stop playback.
    QObject::connect(timer, &QTimer::timeout, [=] {
        int next = slider->value() + delta_i;
        if(next < slider->maximum()) {
            slider->setValue(next);
        } else {
            slider->setValue(slider->maximum());
            stop_playback();
        }
    });

    QObject::connect(slider, &QSlider::valueChanged, [&](int index) {
        this->index = index;
        updateLabels();
        emit indexChanged(index);
    });

    QObject::connect(menu, &QMenu::triggered, [=](QAction *action) {
        stop_playback();
        slider->setValue(action->data().toInt());
    });

    QObject::connect(edit, &QLineEdit::editingFinished, [=, &values] {
        // Todo: This assumes linearly spaced values
        double target = QLocale().toDouble(edit->text());
        double p = (target - values.front())/(values.back() - values.front());
        int index = double(slider->minimum())*(1.0 - p) + double(slider->maximum())*p;

        if(index >= slider->minimum() && index <= slider->maximum()) {
            stop_playback();
            slider->setValue(index);
            emit indexChanged(index);
        }
    });

    QObject::connect(button_play_pause, &QPushButton::clicked, [=] {
        if(timer->isActive()) {
            stop_playback();
        } else {
            if(slider->value() == slider->maximum()) {
                slider->setValue(slider->minimum());
            }
            start_playback();
        }
    });

    QObject::connect(button_skip_backward, &QPushButton::clicked, [=] {
        stop_playback();
        slider->setValue(slider->minimum());
    });

    QObject::connect(button_skip_forward, &QPushButton::clicked, [=] {
        stop_playback();
        slider->setValue(slider->maximum());
    });

    QObject::connect(&quantity, &Quantity::unitChanged, this, &Slider::updateLabels);
    updateLabels();
}

void Slider::addJumpAction(const QString& name, int index) {
    auto action = new QAction(name, this);
    action->setData(index);
    menu->addAction(action);
}

void Slider::updateLabels() {
    double unitValue = quantity.getUnit().fromBase(values[index]);
    edit->setText(QLocale().toString(unitValue));
    label->setText(text + " " + quantity.getUnit().getLabel());
}
