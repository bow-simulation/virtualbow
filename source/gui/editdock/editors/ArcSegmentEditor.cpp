#include "ArcSegmentEditor.hpp"
#include "gui/viewmodel/units/UnitSystem.hpp"

ArcSegmentEditor::ArcSegmentEditor()
    : PropertyValueEditor(2, { "Length", "Radius" }, { &UnitSystem::length, &UnitSystem::length }) {

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
