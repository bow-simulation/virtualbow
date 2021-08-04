#include "UnitSystem.hpp"

UnitSystem::UnitSystem() {
    resetSI();                // Default units on first startup
    loadFromSettings();       // Overwritten by user settings, if present
}

void UnitSystem::loadFromSettings() {
    QSettings settings;
    length.loadFromSettings(settings);
    angle.loadFromSettings(settings);
    mass.loadFromSettings(settings);
    force.loadFromSettings(settings);
    time.loadFromSettings(settings);
    energy.loadFromSettings(settings);
    position.loadFromSettings(settings);
    velocity.loadFromSettings(settings);
    acceleration.loadFromSettings(settings);
    elastic_modulus.loadFromSettings(settings);
    density.loadFromSettings(settings);
    linear_stiffness.loadFromSettings(settings);
    linear_density.loadFromSettings(settings);
    strain.loadFromSettings(settings);
    curvature.loadFromSettings(settings);
    stress.loadFromSettings(settings);
    ratio.loadFromSettings(settings);
    frequency.loadFromSettings(settings);
}

void UnitSystem::saveToSettings() {
    QSettings settings;
    length.saveToSettings(settings);
    angle.saveToSettings(settings);
    mass.saveToSettings(settings);
    force.saveToSettings(settings);
    time.saveToSettings(settings);
    energy.saveToSettings(settings);
    position.saveToSettings(settings);
    velocity.saveToSettings(settings);
    acceleration.saveToSettings(settings);
    elastic_modulus.saveToSettings(settings);
    density.saveToSettings(settings);
    linear_stiffness.saveToSettings(settings);
    linear_density.saveToSettings(settings);
    strain.saveToSettings(settings);
    curvature.saveToSettings(settings);
    stress.saveToSettings(settings);
    ratio.saveToSettings(settings);
    frequency.saveToSettings(settings);
}

void UnitSystem::resetSI() {
    length.setSelectedUnit(Units::Millimeter);
    angle.setSelectedUnit(Units::Degree);
    mass.setSelectedUnit(Units::Gram);
    force.setSelectedUnit(Units::Newton);
    time.setSelectedUnit(Units::Millisecond);
    energy.setSelectedUnit(Units::Joule);
    position.setSelectedUnit(Units::Meter);
    velocity.setSelectedUnit(Units::Meter_Per_Second);
    acceleration.setSelectedUnit(Units::Meter_Per_Second2);
    elastic_modulus.setSelectedUnit(Units::Gigapascal);
    density.setSelectedUnit(Units::Kilogramm_Per_Meter3);
    linear_stiffness.setSelectedUnit(Units::Newton);
    linear_density.setSelectedUnit(Units::Gram_Per_Meter);
    strain.setSelectedUnit(Units::Percent);
    curvature.setSelectedUnit(Units::One_Per_Millimeter);
    stress.setSelectedUnit(Units::Megapascal);
    ratio.setSelectedUnit(Units::Percent);
    frequency.setSelectedUnit(Units::Hertz);
}

void UnitSystem::resetUS() {
    length.setSelectedUnit(Units::Inch);
    angle.setSelectedUnit(Units::Degree);
    mass.setSelectedUnit(Units::Grain);
    force.setSelectedUnit(Units::PoundForce);
    time.setSelectedUnit(Units::Millisecond);
    energy.setSelectedUnit(Units::Foot_Pound);
    position.setSelectedUnit(Units::Foot);
    velocity.setSelectedUnit(Units::Foot_Per_Second);
    acceleration.setSelectedUnit(Units::Foot_Per_Second2);
    elastic_modulus.setSelectedUnit(Units::Psi);
    density.setSelectedUnit(Units::Pound_Per_Foot3);
    linear_stiffness.setSelectedUnit(Units::PoundForce);
    linear_density.setSelectedUnit(Units::Grain_Per_Inch);
    strain.setSelectedUnit(Units::Percent);
    curvature.setSelectedUnit(Units::One_Per_Inch);
    stress.setSelectedUnit(Units::Psi);
    ratio.setSelectedUnit(Units::Percent);
    frequency.setSelectedUnit(Units::Hertz);
}
