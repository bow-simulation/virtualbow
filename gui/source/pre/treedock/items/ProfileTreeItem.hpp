#pragma once
#include "pre/treedock/TreeItem.hpp"
#include "solver/BowModel.hpp"

class ViewModel;
class IntegerPropertyItem;
class DoublePropertyItem;
class SegmentEditor;

class ProfileTreeItem: public TreeItem {
public:
    ProfileTreeItem(ViewModel* model);
    void initFromModel();
};

class SegmentTreeItem: public TreeItem {
public:
    SegmentTreeItem(ViewModel* model, const ProfileSegment& segment);
    ProfileSegment getSegment() const;
    void setSegment(const ProfileSegment& segment);

private:
    QString segmentName(const ProfileSegment& input) const;
    QIcon segmentIcon(const ProfileSegment& input) const;
    SegmentEditor* segmentEditor(const ProfileSegment& input) const;
};
