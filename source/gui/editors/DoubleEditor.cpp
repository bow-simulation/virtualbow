#include "DoubleEditor.hpp"

DoubleEditor::DoubleEditor(const QString& text)
    : text_label(new QLabel(text)),
      unit_label(new QLabel()),
      line_edit(new QLineEdit()),
      changed(false)
{
    text_label->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    line_edit->setValidator(new QDoubleValidator());
    line_edit->setFixedWidth(140);    // Magic number, equal to IntegerEditor

    auto hbox = new QHBoxLayout();
    hbox->setContentsMargins(10, 0, 10, 0);
    hbox->addWidget(text_label, 1);
    hbox->addWidget(unit_label, 0);
    hbox->addWidget(line_edit, 0);
    this->setLayout(hbox);

    QObject::connect(line_edit, &QLineEdit::textEdited, [&]{
        changed = true;
    });

    QObject::connect(line_edit, &QLineEdit::editingFinished, [&]{
        if(changed)
        {
            changed = false;
            emit modified();
        }
    });
}

double DoubleEditor::getData() const
{
    double value = QLocale().toDouble(line_edit->text());
    return unit.toBase(value);
}

void DoubleEditor::setData(double data)
{
    double value = unit.fromBase(data);
    line_edit->setText(QLocale().toString(value, 'g'));
}

void DoubleEditor::setUnit(const Unit& unit)
{
    double value = getData();

    this->unit = unit;
    unit_label->setText(unit.getLabel());

    setData(value);
}
