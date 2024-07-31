#include "SpiralSegmentEditor.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"
#include "pre/utils/DoubleRange.hpp"

SpiralSegmentEditor::SpiralSegmentEditor()
    : PropertyValueEditor(3, { "Length", "R Start", "R End" }, { &Quantities::length, &Quantities::length, &Quantities::length }, { DoubleRange::positive(1e-3), DoubleRange::unrestricted(1e-3), DoubleRange::unrestricted(1e-3) }) {

}

SegmentInput SpiralSegmentEditor::getData() const {
    return getProperties<SpiralConstraint>();
}

void SpiralSegmentEditor::setData(const SegmentInput& data) {
    if(auto value = std::get_if<SpiralInput>(&data)) {
        setProperties<SpiralConstraint>(*value);
    }
    else {
        throw std::invalid_argument("Wrong segment type");
    }
}
