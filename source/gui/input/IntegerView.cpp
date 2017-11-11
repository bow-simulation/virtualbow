#include "IntegerView.hpp"
#include <numeric>

IntegerView::IntegerView(DocumentItem<int>& doc_item)
    : doc_item(doc_item)
{
    this->setMinimum(std::numeric_limits<int>::min());
    this->setMaximum(std::numeric_limits<int>::max());

    this->doc_item.on_value_changed([&]{
        this->setValue(this->doc_item);
    });

    QObject::connect(this, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int value) {
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
