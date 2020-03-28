#include "Slider.hpp"

Slider::Slider(const std::vector<double>& values, const QString& text)
{
    const int HEIGHT = 30;

    auto edit = new QLineEdit();
    edit->setFixedHeight(HEIGHT);
    auto validator = new QDoubleValidator(values.front(), values.back(), 10); // Todo: Magic number
    edit->setValidator(validator);
    //edit->addAction(action_about, QLineEdit::TrailingPosition);

    auto jump_menu = new QMenu();
    jump_menu->addAction("Max. Tension");
    jump_menu->addAction("Max. Compression");
    jump_menu->addAction("Max. String force");

    auto button_jump_to = new QToolButton();
    button_jump_to->setIcon(QIcon(":/icons/media-jump-to.png"));
    button_jump_to->setFixedSize(HEIGHT, HEIGHT);
    button_jump_to->setStyleSheet("QToolButton::menu-indicator { image: none; }");
    button_jump_to->setMenu(jump_menu);
    button_jump_to->setPopupMode(QToolButton::InstantPopup);

    auto button_skip_backward = new QToolButton();
    button_skip_backward->setIcon(QIcon(":/icons/media-skip-backward.png"));
    button_skip_backward->setFixedSize(HEIGHT, HEIGHT);

    auto button_play_pause = new QToolButton();
    button_play_pause->setIcon(QIcon(":/icons/media-playback-start.png"));
    button_play_pause->setFixedSize(HEIGHT, HEIGHT);

    auto button_skip_forward = new QToolButton();
    button_skip_forward->setIcon(QIcon(":/icons/media-skip-forward.png"));
    button_skip_forward->setFixedSize(HEIGHT, HEIGHT);

    auto slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, values.size()-1);

    auto hbox = new QHBoxLayout();
    this->setLayout(hbox);
    hbox->setSpacing(0);
    hbox->addWidget(new QLabel(text));
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

    QObject::connect(slider, &QSlider::valueChanged, [=](int index)
    {
        edit->setText(QLocale().toString(values[index]));
        emit valueChanged(index);
    });

    QObject::connect(edit, &QLineEdit::editingFinished, [=]
    {
        double val= QLocale().toDouble(edit->text());
        double min = values.front();
        double max = values.back();
        double p = (val - min)/(max - min);

        int index = double(slider->minimum())*(1.0 - p) + double(slider->maximum())*p;
        slider->setValue(index);

        emit valueChanged(index);
    });

    emit slider->valueChanged(0);
}
