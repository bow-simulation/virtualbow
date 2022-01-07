#pragma once
#include <QAbstractListModel>
#include "solver/model/profile/ProfileInput.hpp"
#include <QIcon>

class ProfileTreeModel: public QAbstractListModel
{
    Q_OBJECT

public:
    ProfileTreeModel() = default;

    // Model implementation

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    // Additional methods

    void appendSegment(const SegmentInput& segment);
    void insertSegment(const QModelIndex& index, const SegmentInput& segment);

    void removeSegments(const QModelIndexList& indexes);
    void removeSegment();

    void moveSegmentsUp(QModelIndexList indexes);
    void moveSegmentsDown(QModelIndexList indexes);

    const ProfileInput& getData() const;
    void setData(const ProfileInput& data);

signals:
    void modified();

private:
    ProfileInput input;

    QIcon segmentIcon(const SegmentInput& segment) const;
    QString segmentText(const SegmentInput& segment) const;
};
