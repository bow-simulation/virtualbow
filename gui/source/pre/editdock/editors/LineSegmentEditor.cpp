#include "LineSegmentEditor.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"
#include "pre/utils/DoubleRange.hpp"

LineSegmentEditor::LineSegmentEditor()
    : PropertyValueEditor(1, { "Length" }, { &Quantities::length }, { DoubleRange::positive(1e-3) }) {

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
