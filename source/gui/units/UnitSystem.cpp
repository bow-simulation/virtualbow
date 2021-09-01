#include "UnitSystem.hpp"

UnitGroup UnitSystem::length = UnitGroup("Length", {
    Units::Meter,
    Units::Centimeter,
    Units::Millimeter,
    Units::Foot,
    Units::Inch
}, 2, 4);

UnitGroup UnitSystem::angle = UnitGroup("Angle", {
    Units::Radian,
    Units::Degree
}, 1, 1);

UnitGroup UnitSystem::mass = UnitGroup("Mass", {
    Units::Kilogram,
    Units::Gram,
    Units::PoundMass,
    Units::Ounce,
    Units::Grain
}, 1, 4);

UnitGroup UnitSystem::force = UnitGroup("Force", {
    Units::Newton,
    Units::PoundForce
}, 0, 1);

UnitGroup UnitSystem::time = UnitGroup("Time", {
    Units::Second,
    Units::Millisecond
}, 0, 0);

UnitGroup UnitSystem::energy = UnitGroup("Energy",  {
    Units::Joule,
    Units::Foot_Pound
}, 0, 1);

UnitGroup UnitSystem::position = UnitGroup("Position", {
    Units::Meter,
    Units::Centimeter,
    Units::Millimeter,
    Units::Foot,
    Units::Inch
}, 0, 3);

UnitGroup UnitSystem::velocity = UnitGroup("Velocity", {
    Units::Meter_Per_Second,
    Units::Centimeter_Per_Second,
    Units::Millimeter_Per_Second,
    Units::Foot_Per_Second,
    Units::Inch_Per_Second
}, 0, 3);

UnitGroup UnitSystem::acceleration = UnitGroup("Acceleration", {
    Units::Meter_Per_Second2,
    Units::Centimeter_Per_Second2,
    Units::Millimeter_Per_Second2,
    Units::Foot_Per_Second2,
    Units::Inch_Per_Second2
}, 0, 3);

UnitGroup UnitSystem::elastic_modulus = UnitGroup("Elastic modulus", {
    Units::Gigapascal,
    Units::Megapascal,
    Units::Kilopascal,
    Units::Pascal,
    Units::Newton_Per_Millimeter2,
    Units::MegaPsi,
    Units::KiloPsi,
    Units::Psi
}, 0, 7);

UnitGroup UnitSystem::density = UnitGroup("Density", {
    Units::Kilogramm_Per_Meter3,
    Units::Gram_Per_Centimeter3,
    Units::Pound_Per_Foot3
}, 0, 2);

UnitGroup UnitSystem::linear_stiffness = UnitGroup("Linear stiffness", {
    Units::Newton,
    Units::Newton_Per_Percent,
    Units::PoundForce,
    Units::PoundForce_Per_Percent
}, 0, 2);

UnitGroup UnitSystem::linear_density = UnitGroup("Linear density", {
    Units::Kilogramm_Per_Meter,
    Units::Gram_Per_Meter,
    Units::Gram_Per_Centimeter,
    Units::Pound_Per_Foot,
    Units::Pound_Per_Inch,
    Units::Ounce_Per_Foot,
    Units::Ounce_Per_Inch,
    Units::Grain_Per_Foot,
    Units::Grain_Per_Inch
}, 1, 8);

UnitGroup UnitSystem::strain = UnitGroup("Strain", {
    Units::No_Unit,
    Units::Percent
}, 1, 1);

UnitGroup UnitSystem::curvature = UnitGroup("Curvature", {
    Units::One_Per_Meter,
    Units::One_Per_Centimeter,
    Units::One_Per_Millimeter,
    Units::One_Per_Foot,
    Units::One_Per_Inch
}, 2, 4);

UnitGroup UnitSystem::stress = UnitGroup("Stress", {
    Units::Gigapascal,
    Units::Megapascal,
    Units::Kilopascal,
    Units::Pascal,
    Units::Newton_Per_Millimeter2,
    Units::MegaPsi,
    Units::KiloPsi,
    Units::Psi
}, 1, 7);

UnitGroup UnitSystem::ratio = UnitGroup("Ratios and factors", {
    Units::No_Unit,
    Units::Percent
}, 1, 1);

UnitGroup UnitSystem::frequency = UnitGroup("Frequency", {
    Units::Hertz,
    Units::Kilohertz,
    Units::Megahertz
}, 0, 0);

QVector<UnitGroup*> UnitSystem::groups = {
    &length,
    &angle,
    &mass,
    &force,
    &time,
    &energy,
    &position,
    &velocity,
    &acceleration,
    &elastic_modulus,
    &density,
    &linear_stiffness,
    &linear_density,
    &strain,
    &curvature,
    &stress,
    &ratio,
    &frequency
};

void UnitSystem::loadFromSettings(const QSettings& settings) {
    for(auto group: groups) {
        group->loadFromSettings(settings);
    }
}

void UnitSystem::saveToSettings(QSettings& settings) {
    for(auto group: groups) {
        group->saveToSettings(settings);
    }
}

void UnitSystem::resetSI() {
    for(auto group: groups) {
        group->resetSI();
    }
}

void UnitSystem::resetUS() {
    for(auto group: groups) {
        group->resetUS();
    }
}
