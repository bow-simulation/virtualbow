#include "DoubleView.hpp"

DoubleView::DoubleView(DocItem<double>& doc_item)
    : doc_item(doc_item)
{
    connection = doc_item.connect([this](const double& value)
    {
        if(!this->hasFocus())
        {
            setValue(value);
        }
    });

    QObject::connect(this, &QLineEdit::textEdited, [this]
    {
        getValue(false);
    });

    QObject::connect(this, &QLineEdit::editingFinished, [this]
    {
        getValue(true);
    });
}

void DoubleView::setValue(double value)
{
    this->setText(QLocale::c().toString(value, 'g'));    // Todo: Magic number
}

// Todo: Rewrite control structure
void DoubleView::getValue(bool correct)
{
    bool success;
    double value = QLocale::c().toDouble(this->text(), &success);

    if(success)
    {
        try
        {
            this->doc_item = value;
        }
        catch(const std::runtime_error&)
        {
            if(correct)
                setValue(this->doc_item);
        }
    }
    else if(correct)
    {
        setValue(this->doc_item);
    }
}

