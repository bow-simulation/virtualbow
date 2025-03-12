#pragma once
#include "SegmentEditor.hpp"

class TableEditor;

class SplineSegmentEditor: public SegmentEditor
{
public:
    SplineSegmentEditor();

    ProfileSegment getData() const override;
    void setData(const ProfileSegment& data) override;

private:
    TableEditor* table;
};
