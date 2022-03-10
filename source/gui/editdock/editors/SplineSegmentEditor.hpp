#pragma once
#include "SegmentEditor.hpp"

class TableEditor;

class SplineSegmentEditor: public SegmentEditor
{
public:
    SplineSegmentEditor();

    SegmentInput getData() const override;
    void setData(const SegmentInput& data) override;

private:
    TableEditor* table;
};
