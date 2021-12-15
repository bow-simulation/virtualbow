#include "TableModel.hpp"
#include <cmath>

TableModel::TableModel(const QList<QString>& columnLabels, const QList<const UnitGroup*>& columnUnits, int rows, QObject *parent)
    : QAbstractTableModel(parent),
      matrix(MatrixXd::Constant(rows, columnLabels.size(), NAN)),
      columnLabels(columnLabels),
      columnUnits(columnUnits)
{
    if(columnUnits.size() != columnLabels.size()) {
        throw std::invalid_argument("Number of units does not match number of labels");
    }

    // Update table on unit changes
    for(int i = 0; i < columnUnits.size(); ++i) {
        QObject::connect(columnUnits[i], &UnitGroup::selectionChanged, this, [&, i](const Unit& unit) {
            emit headerDataChanged(Qt::Horizontal, i, i);
            emit dataChanged(index(0, i), index(matrix.rows() - 1, i));
        });
    }
}

int TableModel::rowCount(const QModelIndex& parent) const {
    return matrix.rows();
}

int TableModel::columnCount(const QModelIndex& parent) const {
    return matrix.cols();
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
        if(index.isValid() && index.row() < matrix.rows() && index.column() < matrix.cols()) {
            double baseValue = matrix(index.row(), index.column());
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
        if(index.isValid() && index.row() < matrix.rows() && index.column() < matrix.cols()) {
            bool success = true;
            double unitValue = value.toString().isEmpty() ? NAN : value.toString().toDouble(&success);
            if(success) {
                double baseValue = columnUnits[index.column()]->getSelectedUnit().toBase(unitValue);
                matrix(index.row(), index.column()) = baseValue;
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

const MatrixXd& TableModel::getData() const {
    return matrix;
}

void TableModel::setData(const MatrixXd& m) {
    for(int i = 0; i < matrix.rows(); ++i) {
        for(int j = 0; j < matrix.cols(); ++j) {
            if(i < m.rows() && j < m.cols()) {
                matrix(i, j) = m(i, j);
            }
            else {
                matrix(i, j) = NAN;
            }
        }
    }

    emit dataChanged(index(0, 0), index(matrix.rows() - 1, matrix.cols() - 1));
}
