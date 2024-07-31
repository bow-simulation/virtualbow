#include "ArcSegmentEditor.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"
#include "pre/utils/DoubleRange.hpp"

ArcSegmentEditor::ArcSegmentEditor()
    : PropertyValueEditor(2, { "Length", "Radius" }, { &Quantities::length, &Quantities::length }, { DoubleRange::positive(1e-3), DoubleRange::unrestricted(1e-3) }) {

}

SegmentInput ArcSegmentEditor::getData() const {
    return getProperties<ArcConstraint>();
}

void ArcSegmentEditor::setData(const SegmentInput& data) {
    if(auto value = std::get_if<ArcInput>(&data)) {
        setProperties<ArcConstraint>(*value);
    }
    else {
        throw std::invalid_argument("Wrong segment type");
    }
}
