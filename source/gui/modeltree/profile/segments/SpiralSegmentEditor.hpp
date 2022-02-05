#pragma once
#include "PropertyValueEditor.hpp"

class SpiralSegmentEditor: public PropertyValueEditor
{
public:
    SpiralSegmentEditor();

    SegmentInput getData() const override;
    void setData(const SegmentInput& data) override;
};
