#include "TableModel.hpp"
#include <cmath>

TableModel::TableModel(const QString& x_label, const QString& y_label, const UnitGroup& x_unit, const UnitGroup& y_unit, int rows, QObject *parent)
    : QAbstractTableModel(parent),
      points(rows, {NAN, NAN}),
      columnLabels({x_label, y_label}),
      columnUnits({&x_unit, &y_unit})
{
    // Update table on unit changes
    for(int i = 0; i < columnUnits.size(); ++i) {
        QObject::connect(columnUnits[i], &UnitGroup::selectionChanged, this, [&, i](const Unit& unit) {
            emit headerDataChanged(Qt::Horizontal, i, i);
            emit dataChanged(index(0, i), index(points.size() - 1, i));
        });
    }
}

int TableModel::rowCount(const QModelIndex& parent) const {
    return points.size();
}

int TableModel::columnCount(const QModelIndex& parent) const {
    return 2;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        QString name = columnLabels[section];
        QString unit = columnUnits[section]->getSelectedUnit().getLabel();
        QString separator = (!name.isEmpty() && !unit.isEmpty()) ? " " : QString();

        return name + separator + unit;
    }

    return QVariant();
}

QVariant TableModel::data(const QModelIndex& index, int role) const {
    if(role == Qt::DisplayRole || role == Qt::EditRole) {
        if(index.isValid() && index.row() < points.size() && index.column() < 2) {
            double baseValue = points[index.row()](index.column());
            if(!std::isnan(baseValue)) {
                double unitValue = columnUnits[index.column()]->getSelectedUnit().fromBase(baseValue);
                return QString::number(unitValue);
            }
        }
    }

    return QVariant();
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if(role == Qt::EditRole) {
        if(index.isValid() && index.row() < points.size() && index.column() < 2) {
            bool success = true;
            double unitValue = value.toString().isEmpty() ? NAN : value.toString().toDouble(&success);
            if(success) {
                double baseValue = columnUnits[index.column()]->getSelectedUnit().toBase(unitValue);
                points[index.row()](index.column()) = baseValue;
                emit dataChanged(index, index);
                emit modified();
                return true;
            }
        }
    }
    return false;
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

const std::vector<Vector<2>>& TableModel::getData() const {
    return points;
}

void TableModel::setData(const std::vector<Vector<2>>& m) {
    for(int i = 0; i < points.size(); ++i) {
        for(int j = 0; j < 2; ++j) {
            if(i < m.size()) {
                points[i](j) = m[i](j);
            }
            else {
                points[i](j) = NAN;
            }
        }
    }

    emit dataChanged(index(0, 0), index(points.size() - 1, 2 - 1));
}
