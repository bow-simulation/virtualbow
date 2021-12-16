#include "DoubleEditor.hpp"
#include <QLabel>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QHBoxLayout>

DoubleEditor::DoubleEditor(const QString& text, const UnitGroup& group)
    : text_label(new QLabel(text)),
      unit_label(new QLabel()),
      line_edit(new QLineEdit()),
      changed(false),
      unit(group.getSelectedUnit())
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
        if(changed) {
            data = unit.toBase(QLocale().toDouble(line_edit->text()));
            changed = false;
            emit modified();
        }
    });

    QObject::connect(&group, &UnitGroup::selectionChanged, this, [&](const Unit& unit) {
        this->unit = unit;
        update();
    });
}

double DoubleEditor::getData() const {
    return data;
}

void DoubleEditor::setData(double data) {
    this->data = data;
    update();
}

void DoubleEditor::update() {
    unit_label->setText(unit.getLabel());
    line_edit->setText(QLocale().toString(unit.fromBase(data), 'g'));
}
