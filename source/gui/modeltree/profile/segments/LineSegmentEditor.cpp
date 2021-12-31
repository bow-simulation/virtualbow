#include "LineSegmentEditor.hpp"
#include "gui/units/UnitSystem.hpp"

LineSegmentEditor::LineSegmentEditor()
    : PropertyValueEditor(1, { "Length" }, { &UnitSystem::length }) {

}

SegmentInput LineSegmentEditor::getData() const {
    return getProperties<LineConstraint>();
}

void LineSegmentEditor::setData(const SegmentInput& data) {
    if(auto value = std::get_if<LineInput>(&data)) {
        setProperties<LineConstraint>(*value);
    } else {
        throw std::invalid_argument("Wrong segment type");
    }
}
