#pragma once
#include "gui/treeview/TreeItem.hpp"
#include "solver/model/input/InputData.hpp"

class DataViewModel;
class IntegerPropertyItem;
class DoublePropertyItem;
class SegmentEditor;

class ProfileTreeItem: public TreeItem {
public:
    ProfileTreeItem(DataViewModel* model);
    void updateModel();
    void updateView();

    void insertChild(int i, QTreeWidgetItem* item) override;
    void removeChild(int i) override;
    void swapChildren(int i, int j) override;

private:
    DataViewModel* model;
};

class SegmentTreeItem: public TreeItem {
public:
    SegmentTreeItem(const SegmentInput& input);
    SegmentInput getSegment() const;

    //QVariant data(int column, int role) const override;

private:
    QString segmentName(const SegmentInput& input) const;
    QIcon segmentIcon(const SegmentInput& input) const;
    SegmentEditor* segmentEditor(const SegmentInput& input) const;
};
