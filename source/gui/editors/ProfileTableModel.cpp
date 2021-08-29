#include "ProfileTableModel.hpp"
#include <algorithm>

ProfileTableModel::ProfileTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    constraints = {
        Constraint{.type = ConstraintType::X_End, .value = 1.1},
        Constraint{.type = ConstraintType::Y_End, .value = 2.2},
        Constraint{.type = ConstraintType::S_End, .value = 3.3},
    };
}

int ProfileTableModel::rowCount(const QModelIndex& parent) const {
    return constraints.size();
}

int ProfileTableModel::columnCount(const QModelIndex& parent) const {
    return 2;
}

QVariant ProfileTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch(section) {
            case 0: return QString("Type");
            case 1: return QString("Value");
        }
    }
    return QVariant();
}

QVariant ProfileTableModel::data(const QModelIndex& index, int role) const {
    if(role == Qt::DisplayRole || role == Qt::EditRole) {
        if(checkIndex(index)) {
            auto& constraint = constraints[index.row()];
            if(index.column() == 0 && index.row() < constraints.size()) {
                return constraintText(constraint.type);
            }
            if(index.column() == 1 && index.row() < constraints.size()) {
                return constraints[index.row()].value;
            }
        }
    }

    return QVariant();
}

bool ProfileTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role == Qt::EditRole) {
        if (!checkIndex(index)) {
            return false;
        }

        Constraint& constraint = constraints[index.row()];
        switch(index.column()) {
            case 0: {
                return false;
            }
            case 1: {
                constraint.value = value.toDouble();
                return true;
            }
        }
    }
    return false;
}

Qt::ItemFlags ProfileTableModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

// TODO: Replace with built-in method in Qt 5.11 and above
bool ProfileTableModel::checkIndex(const QModelIndex &index) const {
    return (index.row() >= 0 && index.row() < rowCount() && index.column() >= 0 && index.column() < columnCount());
}

QString ProfileTableModel::constraintText(ConstraintType type) const {
    switch(type) {
        case ConstraintType::X_End: return "X (end)";
        case ConstraintType::Y_End: return "Y (end)";
        case ConstraintType::S_End: return "S (end)";
    }
    return QString();
}
