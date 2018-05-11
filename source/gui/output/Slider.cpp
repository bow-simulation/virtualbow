#include "Slider.hpp"

ToolButton::ToolButton(QWidget* parent): QToolButton(parent)
{

}

void ToolButton::paintEvent(QPaintEvent*)
{
    QStylePainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    opt.features &= (~ QStyleOptionToolButton::HasMenu);
    p.drawComplexControl(QStyle::CC_ToolButton, opt);
}

Slider::Slider(const std::vector<double>& values, const QString& text)
{
    auto hbox = new QHBoxLayout();
    this->setLayout(hbox);
    hbox->addWidget(new QLabel(text));

    auto edit = new QLineEdit();
    auto validator = new QDoubleValidator(values.front(), values.back(), 10); // Todo: Magic number
    edit->setValidator(validator);
    hbox->addWidget(edit);

    auto menu = new QMenu();
    menu->addAction("Max. Tension");
    menu->addAction("Max. Compression");
    menu->addAction("Max. String force");

    auto button = new ToolButton();
    button->setEnabled(false);      // Todo
    button->setMenu(menu);
    button->setArrowType(Qt::DownArrow);
    button->setPopupMode(QToolButton::InstantPopup);
    hbox->addWidget(button);
    hbox->addSpacing(15);   // Todo: Magic number

    auto slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, values.size()-1);
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
