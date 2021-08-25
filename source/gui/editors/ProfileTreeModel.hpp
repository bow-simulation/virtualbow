#pragma once
#include "solver/model/ProfileCurve.hpp"
#include <QtWidgets>
#include <vector>

class ProfileTreeModel: public QAbstractTableModel {
    Q_OBJECT

public:
    ProfileTreeModel(QObject *parent = nullptr);
    void insertSegment(const QModelIndexList& selection, const SegmentInput& segment);
    void removeSegments(const QModelIndexList& selection);

    Qt::DropActions supportedDropActions() const override;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    std::vector<SegmentInput> segments;

    QIcon segmentIcon(SegmentType type) const;
    QString segmentText(SegmentType type) const;
};
