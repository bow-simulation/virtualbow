#include "IntegerView.hpp"
#include <numeric>

IntegerView::IntegerView(DocItem<int>& doc_item)
    : doc_item(doc_item)
{
    this->setMinimum(std::numeric_limits<int>::min());
    this->setMaximum(std::numeric_limits<int>::max());

    connection = this->doc_item.connect([this](const unsigned& value)
    {
        this->setValue(value);
    });

    QObject::connect(this, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int value)
    {
        try
        {
            this->doc_item = value;
        }
        catch(const std::runtime_error&)
        {
            this->setValue(this->doc_item);
        }
    });
}
