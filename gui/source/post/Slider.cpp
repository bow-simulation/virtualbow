#include "Slider.hpp"
#include "pre/models/units/Quantity.hpp"
#include "pre/Language.hpp"
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
    edit->setValidator(new QDoubleValidator());

    auto button_jump_to = new QToolButton();
    button_jump_to->setToolTip(Tooltips::SliderJumpTo);
    button_jump_to->setIcon(QIcon(":/icons/media-jump-to.svg"));
    button_jump_to->setFixedSize(height, height);
    button_jump_to->setStyleSheet("QToolButton::menu-indicator { image: none; }");
    button_jump_to->setMenu(menu);
    button_jump_to->setPopupMode(QToolButton::InstantPopup);

    auto button_skip_backward = new QToolButton();
    button_skip_backward->setToolTip(Tooltips::SliderSkipToStart);
    button_skip_backward->setIcon(QIcon(":/icons/media-skip-backward.svg"));
    button_skip_backward->setFixedSize(height, height);

    auto button_play_pause = new QToolButton();
    button_play_pause->setToolTip(Tooltips::SliderPlayPause);
    button_play_pause->setIcon(QIcon(":/icons/media-playback-start.svg"));
    button_play_pause->setFixedSize(height, height);

    auto button_skip_forward = new QToolButton();
    button_skip_forward->setToolTip(Tooltips::SliderSkipToEnd);
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

    std::vector<int> playback_delays(values.size());     // Next playback delay for each index of the value array
    std::vector<int> playback_indices(values.size());    // Next playback index for each index of the value array

    // Determine time delays and next indices for each index of the value array
    for(size_t i = 0; i < values.size(); ++i) {
        // At current index i, search forward by index j until the change in value is sufficient
        // as determined previously by the minimum value step
        for(size_t j = i; j < values.size(); ++j) {
            double value_step = values[j] - values[i];
            if(value_step >= min_valuestep) {
                playback_delays[i] = value_step/time_scaling;
                playback_indices[i] = j;
                break;
            }
        }

        // If no index was assigned, skip to the end instead
        if(playback_indices[i] == 0) {
            playback_indices[i] = values.size() - 1;
        }
    }

    // Set slider range according to the index range of the value array
    slider->setRange(0, values.size()-1);

    // Timer for controlling the playback
    auto timer = new QTimer(this);

    QObject::connect(slider, &QSlider::valueChanged, [=](int index) {
        // Update text and edit labels
        updateLabels();

        // Update timer interval
        timer->setInterval(playback_delays[index]);

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

    QObject::connect(timer, &QTimer::timeout, [=] {
        // Called at the end of a step => advance slider to the next index
        int next_index = playback_indices[slider->value()];
        slider->setValue(next_index);

        // Stop playback if the end is reached
        if(slider->value() == slider->maximum()) {
            stop_playback();
        }
    });

    QObject::connect(menu, &QMenu::triggered, [=](QAction *action) {
        stop_playback();
        slider->setValue(action->data().toInt());
    });

    QObject::connect(edit, &QLineEdit::editingFinished, [=, &values] {
        // Stop playback in case it is running
        stop_playback();

        // Read target value from the edit and convert to base unit
        double unitTarget = QLocale().toDouble(edit->text());
        double target = this->quantity.getUnit().toBase(unitTarget);

        // Out of bounds: Reset label texts and do nothing
        // Otherwise perform a linear search to find the corresponding index to jump to (picks lower bound)
        if(target < values.front()) {
            slider->setValue(slider->minimum());
        }
        else if(target > values.back()) {
            slider->setValue(slider->maximum());
        }
        else {
            for(size_t i = 0; i < values.size(); ++i) {
                if(values[i] <= target && values[i + 1] >= target) {
                    slider->setValue(i);
                    break;
                }
            }
        }

        // This has to be called even though already connected to the value change of the slider
        // The reason is that the slider value might not have changed (i.e. set to the same value as before)
        // but we still want to overwrite the user input in the edit with the actual number.
        updateLabels();
    });

    QObject::connect(button_play_pause, &QPushButton::clicked, [=] {
        if(timer->isActive()) {
            stop_playback();
        }
        else {
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
