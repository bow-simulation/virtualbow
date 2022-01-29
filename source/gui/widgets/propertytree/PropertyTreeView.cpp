#include "PropertyTreeView.hpp"
#include "PropertyTreeModel.hpp"
#include <QHeaderView>

PropertyTreeView::PropertyTreeView(PropertyTreeModel* model) {
    this->setModel(model);
    this->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    this->header()->hide();
    this->expandAll();
}
