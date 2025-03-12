#include "LineSegmentEditor.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"
#include "pre/utils/DoubleRange.hpp"

LineSegmentEditor::LineSegmentEditor()
    : PropertyValueEditor(1, { "Length" }, { &Quantities::length }, { DoubleRange::positive(1e-3) }) {

}

ProfileSegment LineSegmentEditor::getData() const {
    throw std::invalid_argument("Removed code");
    //return getProperties<LineConstraint>();
}

void LineSegmentEditor::setData(const ProfileSegment& data) {
    throw std::invalid_argument("Removed code");
    /*
    if(auto value = std::get_if<Line>(&data)) {
        setProperties<LineConstraint>(*value);
    }
    else {
        throw std::invalid_argument("Wrong segment type");
    }
    */
}
