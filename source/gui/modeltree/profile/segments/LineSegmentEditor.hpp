#pragma once
#include "PropertyValueEditor.hpp"

class LineSegmentEditor: public PropertyValueEditor
{
public:
    LineSegmentEditor();

    SegmentInput getData() const override;
    void setData(const SegmentInput& data) override;
};
