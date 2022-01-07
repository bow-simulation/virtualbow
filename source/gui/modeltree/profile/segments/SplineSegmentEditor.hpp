#pragma once
#include "gui/modeltree/profile/SegmentEditor.hpp"

class TableEditor;

class SplineSegmentEditor: public SegmentEditor
{
public:
    SplineSegmentEditor();

    SegmentInput getData() const override;
    void setData(const SegmentInput& data);

private:
    TableEditor* table;
};
