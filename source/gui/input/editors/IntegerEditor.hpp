#pragma once
#include <QtWidgets>

class IntegerEditor: public QWidget
{
    Q_OBJECT

public:
    IntegerEditor(const QString& text)
        : label(new QLabel(text)),
          edit(new QSpinBox())
    {
        label->setAlignment(Qt::AlignRight | Qt::AlignCenter);
        edit->setFixedWidth(140);    // Magic number, equal to DoubleEditor
        edit->setMinimum(std::numeric_limits<int>::min());
        edit->setMaximum(std::numeric_limits<int>::max());

        auto hbox = new QHBoxLayout();
        hbox->setContentsMargins(10, 0, 10, 0);
        hbox->addWidget(label, 1);
        hbox->addWidget(edit, 0);
        this->setLayout(hbox);

        QObject::connect(edit, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &IntegerEditor::modified);
    }

    void setData(int value)
    {
        edit->setValue(value);
    }

    double getData() const
    {
        return edit->value();
    }

signals:
    void modified();

private:
    QLabel* label;
    QSpinBox* edit;
};
