#include "PropertiesVM.hpp"

PropertiesVM::PropertiesVM(MainVM *parent):
    QAbstractListModel(parent)
{
    QObject::connect(this, &QAbstractItemModel::dataChanged, parent, &MainVM::contentModified);
}

QPersistentModelIndex PropertiesVM::addProperty(AbstractProperty* property) {
    properties.append(property);
    return createIndex(properties.size() - 1, 0);
}

QPersistentModelIndex PropertiesVM::addString(std::string& value) {
    return addProperty(new StringProperty(this, value));
}

QPersistentModelIndex PropertiesVM::addColor(std::string& value) {
    return addProperty(new ColorProperty(this, value));
}

QPersistentModelIndex PropertiesVM::addInteger(int& value) {
    return addProperty(new IntegerProperty(this, value));
}

QPersistentModelIndex PropertiesVM::addDouble(double& value) {
    return addProperty(new DoubleProperty(this, value));
}

int PropertiesVM::rowCount(const QModelIndex& parent) const {
    return properties.count();
}

Qt::ItemFlags PropertiesVM::flags(const QModelIndex& index) const {
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    flags |= Qt::ItemIsEditable;

    return flags;
}

QVariant PropertiesVM::data(const QModelIndex& index, int role) const {
    return properties[index.row()]->data(role);
}

bool PropertiesVM::setData(const QModelIndex& index, const QVariant &value, int role) {
    if(properties[index.row()]->setData(value, role)) {
        emit dataChanged(index, index);
        return true;
    }

    return false;
};
