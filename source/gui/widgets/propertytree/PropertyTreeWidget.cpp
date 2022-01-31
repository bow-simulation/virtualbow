#include "PropertyTreeWidget.hpp"
#include "PropertyDelegate.hpp"
#include <QHeaderView>

PropertyTreeWidget::PropertyTreeWidget() {
    this->setColumnCount(2);
    this->setItemDelegate(new PropertyDelegate2(this));   
    this->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    this->header()->hide();
    //this->expandAll();
}
