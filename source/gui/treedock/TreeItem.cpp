#include "TreeItem.hpp"

TreeItem::TreeItem(const QString& name, const QIcon& icon, TreeItemType type)
    : QTreeWidgetItem({name}, type),
      editor(nullptr),
      plot(nullptr)
{
    this->setIcon(0, icon);
}

QWidget* TreeItem::getEditor() {
    return editor;
}

QWidget* TreeItem::getPlot() {
    return plot;
}

void TreeItem::setEditor(QWidget* editor) {
    this->editor = editor;
}

void TreeItem::setPlot(QWidget* plot) {
    this->plot = plot;
}

void TreeItem::insertChild(int i, QTreeWidgetItem* item) {
    if(i >= 0 && i <= this->childCount()) {
        QTreeWidgetItem::insertChild(i, item);
    }
}

void TreeItem::removeChild(int i) {
    if(i >= 0 && i < this->childCount()) {
        QTreeWidgetItem::removeChild(this->child(i));
    }
}

void TreeItem::swapChildren(int i, int j) {
    int i_min = std::min(i, j);
    int i_max = std::max(i, j);

    if(i_min >= 0 && i_max < this->childCount()) {
        QTreeWidgetItem* current = this->treeWidget()->currentItem();    // Remember selected item
        QTreeWidgetItem* item_max = this->takeChild(i_max);
        QTreeWidgetItem* item_min = this->takeChild(i_min);

        QTreeWidgetItem::insertChild(i_min, item_max);
        QTreeWidgetItem::insertChild(i_max, item_min);
        this->treeWidget()->setCurrentItem(current);    // Reset previously selected item
    }
}

int TreeItem::row() const {
    auto parent = this->parent();
    if(parent != nullptr) {
        return parent->indexOfChild(const_cast<TreeItem*>(this));
    }

    return 0;
}
