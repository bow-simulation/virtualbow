#pragma once
#include "PropertyValueEditor.hpp"

class ArcSegmentEditor: public PropertyValueEditor
{
public:
    ArcSegmentEditor();

    ProfileSegment getData() const override;
    void setData(const ProfileSegment& data) override;
};
