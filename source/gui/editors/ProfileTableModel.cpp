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
        if(section == 0) {
            return QString("Type");
        }
        else {
            return QString("Value");
        }
    }
    return QVariant();
}

QVariant ProfileTableModel::data(const QModelIndex& index, int role) const {
    switch(role) {
        case Qt::DisplayRole:
            if(index.row() < constraints.size()) {
                auto& constraint = constraints[index.row()];
                if(index.column() == 0 && index.row() < constraints.size()) {
                    return constraintText(constraint.type);
                }
                if(index.column() == 1 && index.row() < constraints.size()) {
                    return QString::number(constraints[index.row()].value);
                }
            }
    }

    return QVariant();
}

QString ProfileTableModel::constraintText(ConstraintType type) const {
    switch(type) {
        case ConstraintType::X_End: return "X (end)";
        case ConstraintType::Y_End: return "Y (end)";
        case ConstraintType::S_End: return "S (end)";
    }
    return QString();
}
