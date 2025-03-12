#include "SpiralSegmentEditor.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"
#include "pre/utils/DoubleRange.hpp"

SpiralSegmentEditor::SpiralSegmentEditor()
    : PropertyValueEditor(3, { "Length", "R Start", "R End" }, { &Quantities::length, &Quantities::length, &Quantities::length }, { DoubleRange::positive(1e-3), DoubleRange::unrestricted(1e-3), DoubleRange::unrestricted(1e-3) }) {

}

ProfileSegment SpiralSegmentEditor::getData() const {
    throw std::invalid_argument("Removed code");
    //return getProperties<SpiralConstraint>();
}

void SpiralSegmentEditor::setData(const ProfileSegment& data) {
    throw std::invalid_argument("Removed code");
    /*
    if(auto value = std::get_if<Spiral>(&data)) {
        setProperties<SpiralConstraint>(*value);
    }
    else {
        throw std::invalid_argument("Wrong segment type");
    }
    */
}
