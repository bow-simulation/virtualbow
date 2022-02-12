#include "TreeItem.hpp"

#include <QLabel>

TreeItem::TreeItem(const QString& name, const QIcon& icon, TreeItemType type)
    : QTreeWidgetItem({name}, type)
{
    this->setIcon(0, icon);
}

QWidget* TreeItem::getEditor() {
    if(editor == nullptr) {
        editor = createEditor();
    }
    return editor;
}

QWidget* TreeItem::getPlot() {
    if(plot == nullptr) {
        plot = createPlot();
    }
    return plot;
}

QWidget* TreeItem::createEditor() const {
    return nullptr;
}

QWidget* TreeItem::createPlot() const {
    return nullptr;
}
