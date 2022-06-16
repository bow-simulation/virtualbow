#include "LineSegmentEditor.hpp"
#include "gui/viewmodel/units/UnitSystem.hpp"
#include "gui/utils/DoubleRange.hpp"

LineSegmentEditor::LineSegmentEditor()
    : PropertyValueEditor(1, { "Length" }, { &UnitSystem::length }, { DoubleRange::positive(1e-3) }) {

}

SegmentInput LineSegmentEditor::getData() const {
    return getProperties<LineConstraint>();
}

void LineSegmentEditor::setData(const SegmentInput& data) {
    if(auto value = std::get_if<LineInput>(&data)) {
        setProperties<LineConstraint>(*value);
    }
    else {
        throw std::invalid_argument("Wrong segment type");
    }
}
