#pragma once
#include "solver/model/ProfileCurve.hpp"
#include <QtWidgets>

class ProfileTableModel: public QAbstractTableModel {
    Q_OBJECT

public:
    ProfileTableModel(QObject *parent = nullptr);

    // Data display
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    std::vector<Constraint> constraints;

    QString constraintText(ConstraintType type) const;
};
