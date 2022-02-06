#include "TreeItem.hpp"

#include <QLabel>

TreeItem::TreeItem(const QString& name, const QIcon& icon, TreeItemType type)
    : QTreeWidgetItem({name}, type)
{
    this->setIcon(0, icon);
}

QWidget* TreeItem::getEditor() {
    if(editor == nullptr) {
        editor = new QLabel(this->text(0));
    }
    return editor;
}

QWidget* TreeItem::getPlot() {
    if(plot == nullptr) {
        plot = new QLabel(this->text(0));
    }
    return plot;
}
