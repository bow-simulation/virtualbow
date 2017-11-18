#include "DoubleView.hpp"

DoubleView::DoubleView(DocumentItem<double>& doc_item)
    : doc_item(doc_item)
{
    QObject::connect(&doc_item, &DocumentNode::value_changed, this, &DoubleView::update);
    QObject::connect(this, &QLineEdit::textEdited, [this]{ getValue(false); });
    QObject::connect(this, &QLineEdit::editingFinished, [this]{ getValue(true); });

    setValue(this->doc_item);
}

void DoubleView::update()
{
    if(!this->hasFocus())
        setValue(this->doc_item);
}

void DoubleView::setValue(double value)
{
    this->setText(QLocale::c().toString(value, 'g'));
}

void DoubleView::getValue(bool correct)
{
    bool success;
    double value = QLocale::c().toDouble(this->text(), &success);

    if(success)
    {
        this->doc_item = value;
    }
    else if(correct)
    {
        setValue(this->doc_item);
    }
}

