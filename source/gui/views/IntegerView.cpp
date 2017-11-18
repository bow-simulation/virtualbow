#include "IntegerView.hpp"
#include <numeric>

IntegerView::IntegerView(DocumentItem<int>& doc_item)
    : doc_item(doc_item)
{
    this->setMinimum(std::numeric_limits<int>::min());
    this->setMaximum(std::numeric_limits<int>::max());
    // this->setAutoFillBackground(true);    // https://stackoverflow.com/a/1572358/4692009

    QObject::connect(this, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int value) {
            this->doc_item = value;
    });

    QObject::connect(&doc_item, &DocumentNode::value_changed, this, &IntegerView::update_value);
    QObject::connect(&doc_item, &DocumentNode::error_changed, this, &IntegerView::update_error);

    update_value();
    update_error();
}

void IntegerView::update_value()
{
    if(!this->hasFocus())
        this->setValue(doc_item);
}

void IntegerView::update_error()
{
    if(doc_item.get_errors().size() == 0)
    {
        QPalette palette;
        palette.setColor(QPalette::Base, Qt::white);
        this->setPalette(palette);
        this->setToolTip("");
    }
    else
    {
        QPalette palette;
        palette.setColor(QPalette::Base, QColor(255, 102, 102));    // Magic numbers
        this->setPalette(palette);
        this->setToolTip(QString::fromStdString(doc_item.get_errors().front()));
    }
}
