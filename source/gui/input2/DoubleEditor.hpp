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
        edit->setValidator(new QDoubleValidator());

        auto hbox = new QHBoxLayout();
        hbox->addWidget(label);
        hbox->addWidget(edit);
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
