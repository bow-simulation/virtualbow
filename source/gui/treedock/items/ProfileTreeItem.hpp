#pragma once
#include "gui/treedock/TreeItem.hpp"
#include "solver/model/input/InputData.hpp"

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
    SegmentTreeItem(ViewModel* model, const SegmentInput& segment);
    SegmentInput getSegment() const;
    void setSegment(const SegmentInput& segment);

private:
    QString segmentName(const SegmentInput& input) const;
    QIcon segmentIcon(const SegmentInput& input) const;
    SegmentEditor* segmentEditor(const SegmentInput& input) const;
};
