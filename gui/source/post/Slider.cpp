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
      quantity(quantity)
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

    double time_scaling = (values.back() - values.front())/PLAYBACK_PERIOD_MS;    // Change in value per playback time
    double min_timestep = 1000.0/double(PLAYBACK_MAX_FPS);                        // Minimum bound on the playback timestep in ms as defined by the FPS
    double min_valuestep = min_timestep*time_scaling;                             // Minimum change in value per playback step

    qInfo() << min_valuestep;

    std::vector<int> timer_delays(values.size());    // Next playback delay for each index of the value array
    std::vector<int> timer_steps(values.size());     // Next playback step size for each index of the value array

    for(size_t i = 0; i < values.size(); ++i) {
        // At current index i, search forward by index j
        for(size_t j = i; j < values.size(); ++j) {
            double value_step = values[j] - values[i];
            if(value_step >= min_valuestep) {
                timer_delays[i] = value_step/time_scaling;
                timer_steps[i] = j - i;
                break;
            }
        }
    }

    qInfo() << "==================================================";
    for(size_t i = 0; i < values.size(); ++i) {
        qInfo() << values[i] << timer_delays[i] << timer_steps[i];
    }





    // Timer interval and number of steps done by the timer each tick
    //int delta_i = std::ceil(1000*double(values.size() - 1)/(PLAYBACK_PERIOD*PLAYBACK_MAX_FPS));

    //int delta_i = 1;
    //int delta_t = PLAYBACK_PERIOD_MS*delta_i/(values.size() - 1);

    // Set slider range according to the index range of the value array
    slider->setRange(0, values.size()-1);

    auto timer = new QTimer(this);

    QObject::connect(slider, &QSlider::valueChanged, [=, &values](int index) {
        // Update text and edit labels
        updateLabels();

        // Update timer interval and stepsize
        timer->setInterval(timer_delays[index]);

        emit indexChanged(index);
    });

    auto start_playback = [=] {
        timer->start();
        button_play_pause->setIcon(QIcon(":/icons/media-playback-pause.svg"));
    };

    auto stop_playback = [=] {
        timer->stop();
        button_play_pause->setIcon(QIcon(":/icons/media-playback-start.svg"));
    };

    // Timer: Advance slider by delta_i if possible, else skip the last bit and stop playback.
    QObject::connect(timer, &QTimer::timeout, [=, &values] {
        // Called at the end of a step => advance slider by timestep at current index, if possible (step != 0)
        int step = timer_steps[slider->value()];
        if(step == 0) {
            stop_playback();
        }
        else {
            slider->setValue(slider->value() + step);
        }


        /*
        int next = slider->value() + timer_steps[slider->value()];
        if(next < slider->maximum()) {
            slider->setValue(next);
        } else {
            slider->setValue(slider->maximum());
            stop_playback();
        }
        */
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

    // Emit valueChanged once to initialize everything
    // (slider->setValue(0) does not work because the slider is already at zero)
    emit slider->valueChanged(slider->value());
}

void Slider::addJumpAction(const QString& name, int index) {
    auto action = new QAction(name, this);
    action->setData(index);
    menu->addAction(action);
}

void Slider::updateLabels() {
    double unitValue = quantity.getUnit().fromBase(values[slider->value()]);
    edit->setText(QLocale().toString(unitValue));
    label->setText(text + " " + quantity.getUnit().getLabel());
}
