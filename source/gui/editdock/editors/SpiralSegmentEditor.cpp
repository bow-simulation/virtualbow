#include "SpiralSegmentEditor.hpp"
#include "gui/viewmodel/units/UnitSystem.hpp"

SpiralSegmentEditor::SpiralSegmentEditor()
    : PropertyValueEditor(3, { "Length", "Curv. Start", "Curv. End" }, { &UnitSystem::length, &UnitSystem::curvature, &UnitSystem::curvature }) {

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
