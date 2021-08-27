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

    // Data display
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Drag and drop
    Qt::DropActions supportedDropActions() const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    QStringList mimeTypes() const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex& parent);
    QMimeData* mimeData(const QModelIndexList &indexes) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

private:
    std::vector<SegmentInput> segments;

    QIcon segmentIcon(SegmentType type) const;
    QString segmentText(SegmentType type) const;
};
