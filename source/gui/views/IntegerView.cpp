#include "IntegerView.hpp"
#include <numeric>

IntegerView::IntegerView(DocumentItem<int>& doc_item)
    : doc_item(doc_item)
{
    this->setMinimum(std::numeric_limits<int>::min());
    this->setMaximum(std::numeric_limits<int>::max());

    QObject::connect(&doc_item, &DocumentNode::modified, this, &IntegerView::update);

    QObject::connect(this, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int value) {
            this->doc_item = value;
    });

    update();
}

void IntegerView::update()
{
    if(!this->hasFocus())
        this->setValue(this->doc_item);
}
