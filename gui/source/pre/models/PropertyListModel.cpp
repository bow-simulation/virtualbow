#include "PropertyListModel.hpp"

PropertyListModel::PropertyListModel() {
    // Emit modified signal if the model data has changed
    QObject::connect(this, &QAbstractItemModel::dataChanged, this, &PropertyListModel::contentModified);
}

QPersistentModelIndex PropertyListModel::addProperty(AbstractProperty* property) {
    properties.append(property);
    return createIndex(properties.size() - 1, 0);
}

QPersistentModelIndex PropertyListModel::addString(std::string& value) {
    return addProperty(new StringProperty(this, value));
}

QPersistentModelIndex PropertyListModel::addColor(std::string& value) {
    return addProperty(new ColorProperty(this, value));
}

QPersistentModelIndex PropertyListModel::addInteger(int& value) {
    return addProperty(new IntegerProperty(this, value));
}

QPersistentModelIndex PropertyListModel::addDouble(double& value) {
    return addProperty(new DoubleProperty(this, value));
}

int PropertyListModel::rowCount(const QModelIndex& parent) const {
    return properties.count();
}

Qt::ItemFlags PropertyListModel::flags(const QModelIndex& index) const {
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    flags |= Qt::ItemIsEditable;

    return flags;
}

QVariant PropertyListModel::data(const QModelIndex& index, int role) const {
    return properties[index.row()]->data(role);
}

bool PropertyListModel::setData(const QModelIndex& index, const QVariant &value, int role) {
    if(properties[index.row()]->setData(value, role)) {
        emit dataChanged(index, index);
        return true;
    }

    return false;
};
