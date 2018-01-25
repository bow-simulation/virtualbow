#pragma once
#include <QtWidgets>

class DoubleEditor: public QWidget
{
    Q_OBJECT

public:
    DoubleEditor(const QString& text)
        : label(new QLabel(text)),
          edit(new QLineEdit()),
          changed(false)
    {
        label->setAlignment(Qt::AlignRight);
        edit->setValidator(new QDoubleValidator());
        edit->setFixedWidth(140);    // Magic number, equal to IntegerEditor

        auto hbox = new QHBoxLayout();
        hbox->setContentsMargins(10, 0, 10, 0);
        hbox->addWidget(label, 1);
        hbox->addWidget(edit, 0);
        this->setLayout(hbox);

        QObject::connect(edit, &QLineEdit::textEdited, [&]{
            changed = true;
        });

        QObject::connect(edit, &QLineEdit::editingFinished, [&]{
            if(changed)
            {
                changed = false;
                emit modified();
            }
        });
    }

    void setData(double data)
    {
        edit->setText(QLocale::c().toString(data, 'g'));
    }

    double getData() const
    {
        return QLocale::c().toDouble(edit->text());
    }

signals:
    void modified();

private:
    QLabel* label;
    QLineEdit* edit;
    bool changed;
};
