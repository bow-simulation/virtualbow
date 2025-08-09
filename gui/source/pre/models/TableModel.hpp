#pragma once
#include "solver/BowModel.hpp"
#include <QAbstractTableModel>

class Quantity;

class TableModel: public QAbstractTableModel {
    Q_OBJECT

public:
    TableModel(Points& points, const QString& xLabel, const QString& yLabel, const Quantity& xQuantity, const Quantity& yQuantity);

    // Implementation of QAbstractItemModel

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

signals:
    void contentModified();

private:
    QList<QString> columnLabels;
    QList<const Quantity*> columnUnits;
    QMap<QModelIndex, double> entries;
    Points& points;
    int loadedRows;

    Points getPoints() const;
    void setPoints(const Points& data);
};
