#include "TreeItem.hpp"
#include "gui/viewmodel/ViewModel.hpp"

TreeItem::TreeItem(ViewModel* model, const QString& name, const QIcon& icon, TreeItemType type)
    : QTreeWidgetItem({name}, type),
      model(model),
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

void TreeItem::removeChild(int i) {
    if(i >= 0 && i < this->childCount()) {
        QTreeWidgetItem::removeChild(this->child(i));
    }
}

void TreeItem::removeChildren() {
    while(this->childCount() > 0) {
        QTreeWidgetItem::removeChild(this->child(0));
    }
}

/*
QVariant TreeItem::data(int column, int role) const {
    if(role == Qt::DisplayRole) {
        QString text = QTreeWidgetItem::data(column, role).toString();    // Original text of this item

        int total_count = 0;    // Number of items under the parent with the same name
        int display_count = 0;    // Number of items with the same name until (and including) this item

        if(this->parent() != nullptr) {
            for(int i = 0; i < this->parent()->childCount(); ++i) {
                if(this->parent()->child(i)->QTreeWidgetItem::data(column, role) == text) {
                    total_count += 1;
                    if(i <= this->row()) {
                        display_count += 1;
                    }
                }
            }
        }

        // If at least two items have the same name, show the display count after the original item text
        if(total_count > 1) {
            return text + " " + QString::number(display_count);
        }
        else {
            return text;
        }
    }

    return QTreeWidgetItem::data(column, role);
}
*/

int TreeItem::row() const {
    auto parent = this->parent();
    if(parent != nullptr) {
        return parent->indexOfChild(const_cast<TreeItem*>(this));
    }

    return 0;
}
