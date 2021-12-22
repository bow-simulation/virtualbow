#pragma once
#include <QAbstractTableModel>
#include "solver/numerics/EigenTypes.hpp"
#include "gui/units/UnitGroup.hpp"

class TableModel: public QAbstractTableModel {
    Q_OBJECT

public:
    TableModel(const QString& x_label, const QString& y_label, const UnitGroup& x_unit, const UnitGroup& y_unit, int rows, QObject *parent = nullptr);

    // Implementattion of QAbstractItemModel

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // Additional methods
    const std::vector<Vector<2>>& getData() const;
    void setData(const std::vector<Vector<2>>& matrix);

signals:
    void modified();

private:
    std::vector<Vector<2>> points;
    QList<QString> columnLabels;
    QList<const UnitGroup*> columnUnits;
};
