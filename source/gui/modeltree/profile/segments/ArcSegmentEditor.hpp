#pragma once
#include "PropertyValueEditor.hpp"

class ArcSegmentEditor: public PropertyValueEditor
{
public:
    ArcSegmentEditor();

    SegmentInput getData() const override;
    void setData(const SegmentInput& data);
};
