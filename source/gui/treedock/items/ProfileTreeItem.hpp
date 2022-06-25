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
    void updateModel(void* source) override;
    void updateView(void* source) override;
};

class SegmentTreeItem: public TreeItem {
public:
    SegmentTreeItem(ViewModel* model, const SegmentInput& input);
    SegmentInput getSegment() const;

private:
    QString segmentName(const SegmentInput& input) const;
    QIcon segmentIcon(const SegmentInput& input) const;
    SegmentEditor* segmentEditor(const SegmentInput& input) const;
};
