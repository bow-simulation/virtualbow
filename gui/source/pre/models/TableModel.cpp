#include "TableModel.hpp"
#include "pre/models/MainModel.hpp"
#include "pre/models/units/Quantity.hpp"
#include <cmath>

const int INITIAL_ROWS = 25;    // Initial number of rows in the table
const int DELTA_ROWS = 5;       // Number of rows to add when the end is reached

TableModel::TableModel(Points& points, const QString& xLabel, const QString& yLabel, const Quantity& xQuantity, const Quantity& yQuantity):
    columnLabels({xLabel, yLabel}),
    columnUnits({&xQuantity, &yQuantity}),
    loadedRows(INITIAL_ROWS),
    points(points)
{
    // Update table on units changes
    for(int i = 0; i < columnUnits.size(); ++i) {
        QObject::connect(columnUnits[i], &Quantity::unitChanged, this, [&, i](const Unit& quantity) {
            emit headerDataChanged(Qt::Horizontal, i, i);
            emit dataChanged(index(0, i), index(rowCount() - 1, i));  // TODO: Only those that actually changed?
        });
    }

    setPoints(points);
    QObject::connect(this, &TableModel::contentModified, this, [=]{
        this->points = getPoints();
    });
}

int TableModel::rowCount(const QModelIndex& parent) const {
    return loadedRows;
}

int TableModel::columnCount(const QModelIndex& parent) const {
    return 2;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        QString name = columnLabels[section];
        QString unit = columnUnits[section]->getUnit().getSuffix();
        QString separator = (!name.isEmpty() && !unit.isEmpty()) ? " " : QString();

        return name + separator + unit;
    }

    return QVariant();
}

// Return unit value for display and base value for editing (editor handles units itself)
QVariant TableModel::data(const QModelIndex& index, int role) const {
    if(role == Qt::DisplayRole && index.isValid() && entries.contains(index)) {
        double baseValue = entries.value(index);
        return columnUnits[index.column()]->getUnit().fromBase(baseValue);
    }

    if(role == Qt::EditRole && index.isValid() && entries.contains(index)) {
        return entries.value(index);  // Edit widgets handle units themselves
    }

    return QVariant();
}

bool TableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(index.isValid()) {
        if(value.isNull()) {
            entries.remove(index);
        }
        else {
            if(role == Qt::DisplayRole) {
                double baseValue = columnUnits[index.column()]->getUnit().toBase(value.toDouble());
                entries.insert(index, baseValue);
            }
            else if(role == Qt::EditRole) {
                entries.insert(index, value.toDouble());  // Edit widgets handle units themselves
            }
        }

        emit dataChanged(index, index);
        emit contentModified();
        return true;
    }

    return false;
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const {
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

bool TableModel::canFetchMore(const QModelIndex& parent) const {
    if(parent.isValid()) {
        return false;
    }
    return true;
}

void TableModel::fetchMore(const QModelIndex& parent) {
    beginInsertRows(QModelIndex(), loadedRows, loadedRows + DELTA_ROWS - 1);
    loadedRows += DELTA_ROWS;
    endInsertRows();
}

Points TableModel::getPoints() const {
    Points data;
    data.reserve(2*entries.size());    // Upper bound on number of valid data points

    // Since the indices are stored in a QMap, they are sorted after row and column
    // Iterate over the map and convert entries into rows

    auto it_row_begin = entries.cbegin();
    auto it_row_end = entries.cbegin();

    while(it_row_begin != entries.cend()) {
        while(it_row_end.key().row() == it_row_begin.key().row() && it_row_end != entries.cend()) {
            ++it_row_end;
        }

        if(std::distance(it_row_begin, it_row_end) == 2) {
            data.push_back({
                (it_row_begin + 0).value(),
                (it_row_begin + 1).value()
            });
        }

        it_row_begin = it_row_end;
    }

    return data;
}

void TableModel::setPoints(const Points& data) {
    // Inser new rows at the end if needed
    if(data.size() > loadedRows) {
        beginInsertRows(QModelIndex(), loadedRows, data.size() - 1);
        loadedRows = data.size();
        endInsertRows();
    }

    // Remove previous entries, keep track of all cells that potentially changed
    QSet<QModelIndex> changed(entries.keyBegin(), entries.keyEnd());
    entries.clear();

    // Create new entries, also add them to the change set
    for(size_t i = 0; i < data.size(); ++i) {
        for(size_t j = 0; j < 2; ++j) {
            QModelIndex index = this->index(i, j);
            entries.insert(index, data[i][j]);
            changed.insert(index);
        }
    }

    // Update changed cells
    for(auto& index: changed) {
        emit dataChanged(index, index);
    }
}
