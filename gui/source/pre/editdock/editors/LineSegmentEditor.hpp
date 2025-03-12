#pragma once
#include "PropertyValueEditor.hpp"

class LineSegmentEditor: public PropertyValueEditor
{
public:
    LineSegmentEditor();

    ProfileSegment getData() const override;
    void setData(const ProfileSegment& data) override;
};
