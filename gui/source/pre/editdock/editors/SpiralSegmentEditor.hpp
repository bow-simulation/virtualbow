#pragma once
#include "PropertyValueEditor.hpp"

class SpiralSegmentEditor: public PropertyValueEditor
{
public:
    SpiralSegmentEditor();

    ProfileSegment getData() const override;
    void setData(const ProfileSegment& data) override;
};
