#pragma once
#include <QAbstractTableModel>
#include "solver/numerics/EigenTypes.hpp"
#include "gui/viewmodel/units/UnitGroup.hpp"

class TableModel: public QAbstractTableModel {
    Q_OBJECT

public:
    TableModel(const QString& x_label, const QString& y_label, const UnitGroup& x_unit, const UnitGroup& y_unit, QObject *parent = nullptr);

    // Implementation of QAbstractItemModel

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

    // Additional methods

    std::vector<Vector<2>> getData() const;
    void setData(const std::vector<Vector<2>>& data);

signals:
    void modified();

private:
    QList<QString> columnLabels;
    QList<const UnitGroup*> columnUnits;
    QMap<QModelIndex, double> entries;
    int loadedRows;
};
