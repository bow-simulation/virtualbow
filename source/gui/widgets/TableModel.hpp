#pragma once
#include <QAbstractTableModel>
#include "solver/numerics/EigenTypes.hpp"
#include "gui/units/UnitGroup.hpp"

class TableModel: public QAbstractTableModel {
    Q_OBJECT

public:
    TableModel(const QList<QString>& columnLabels, const QList<const UnitGroup*>& columnUnits, int rows, QObject *parent = nullptr);

    // Implementattion of QAbstractItemModel

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // Additional methods
    const MatrixXd& getData() const;
    void setData(const MatrixXd& matrix);

signals:
    void modified();

private:
    MatrixXd matrix;
    QList<QString> columnLabels;
    QList<const UnitGroup*> columnUnits;
};
