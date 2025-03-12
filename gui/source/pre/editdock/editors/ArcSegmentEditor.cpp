#include "ArcSegmentEditor.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"
#include "pre/utils/DoubleRange.hpp"

ArcSegmentEditor::ArcSegmentEditor()
    : PropertyValueEditor(2, { "Length", "Radius" }, { &Quantities::length, &Quantities::length }, { DoubleRange::positive(1e-3), DoubleRange::unrestricted(1e-3) }) {

}

ProfileSegment ArcSegmentEditor::getData() const {
    throw std::invalid_argument("Removed code");
    //return getProperties<ArcConstraint>();
}

void ArcSegmentEditor::setData(const ProfileSegment& data) {
    throw std::invalid_argument("Removed code");
    /*
    if(auto value = std::get_if<Arc>(&data)) {
        setProperties<ArcConstraint>(*value);
    }
    else {
        throw std::invalid_argument("Wrong segment type");
    }
    */
}
