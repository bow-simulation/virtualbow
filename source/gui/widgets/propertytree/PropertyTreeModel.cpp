#include "PropertyTreeModel.hpp"
#include "PropertyTreeItem.hpp"

PropertyTreeModel::PropertyTreeModel(PropertyTreeItem* root, QObject* parent)
    : QAbstractItemModel(parent),
      root(root)
{

}

PropertyTreeModel::~PropertyTreeModel() {
    delete root;
}

QModelIndex PropertyTreeModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    PropertyTreeItem* parentItem;
    if(!parent.isValid()) {
        parentItem = root;
    }
    else {
        parentItem = static_cast<PropertyTreeItem*>(parent.internalPointer());
    }

    PropertyTreeItem *childItem = parentItem->getChildren().value(row, nullptr);
    if(childItem) {
        return createIndex(row, column, childItem);
    }

    return QModelIndex();
}

QModelIndex PropertyTreeModel::parent(const QModelIndex& index) const {
    if(!index.isValid()) {
        return QModelIndex();
    }

    PropertyTreeItem* childItem = static_cast<PropertyTreeItem*>(index.internalPointer());
    PropertyTreeItem* parentItem = childItem->getParent();

    if(parentItem == root) {
        return QModelIndex();
    }

    int row = parentItem->getChildren().indexOf(childItem);
    return createIndex(row, 0, parentItem);
}

int PropertyTreeModel::rowCount(const QModelIndex& parent) const {
    PropertyTreeItem* parentItem;
    if(parent.column() > 0) {
        return 0;
    }

    if(!parent.isValid()) {
        parentItem = root;
    }
    else {
        parentItem = static_cast<PropertyTreeItem*>(parent.internalPointer());
    }

    return parentItem->getChildren().size();
}

int PropertyTreeModel::columnCount(const QModelIndex& parent) const {
    return 2;
}

QVariant PropertyTreeModel::data(const QModelIndex& index, int role) const {
    if(index.isValid() && role == Qt::DisplayRole) {
        PropertyTreeItem *item = static_cast<PropertyTreeItem*>(index.internalPointer());
        if(index.column() == 0) {
            return item->getName();
        }
        if(index.column() == 1) {
            return item->getValue();
        }
    }

    return QVariant();
}

Qt::ItemFlags PropertyTreeModel::flags(const QModelIndex& index) const {
    if(!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return QAbstractItemModel::flags(index);
}

QVariant PropertyTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if(section == 0) {
            return "Property";
        }
        if(section == 1) {
            return "Value";
        }
    }

    return QVariant();
}
