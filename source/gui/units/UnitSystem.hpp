#pragma once
#include "Definitions.hpp"
#include "UnitGroup.hpp"

struct UnitSystem {

    UnitSystem();

    void loadFromSettings();
    void saveToSettings();
    void resetDefaults();

    UnitGroup length = UnitGroup("Length", {
        Units::Meter,
        Units::Centimeter,
        Units::Millimeter,
        Units::Foot,
        Units::Inch
    });

    UnitGroup angle = UnitGroup("Angle", {
        Units::Radian,
        Units::Degree
    });

    UnitGroup mass = UnitGroup("Mass", {
        Units::Kilogram,
        Units::Gram,
        Units::PoundMass,
        Units::Ounce,
        Units::Grain
    });

    UnitGroup force = UnitGroup("Force", {
        Units::Newton,
        Units::PoundForce
    });

    UnitGroup time = UnitGroup("Time", {
        Units::Second,
        Units::Millisecond
    });

    UnitGroup energy = UnitGroup("Energy",  {
        Units::Joule,
        Units::Foot_Pound
    });

    UnitGroup position = UnitGroup("Position", {
        Units::Meter,
        Units::Centimeter,
        Units::Millimeter,
        Units::Foot,
        Units::Inch
    });

    UnitGroup velocity = UnitGroup("Velocity", {
        Units::Meter_Per_Second,
        Units::Centimeter_Per_Second,
        Units::Millimeter_Per_Second,
        Units::Foot_Per_Second,
        Units::Inch_Per_Second
    });

    UnitGroup acceleration = UnitGroup("Acceleration", {
        Units::Meter_Per_Second2,
        Units::Centimeter_Per_Second2,
        Units::Millimeter_Per_Second2,
        Units::Foot_Per_Second2,
        Units::Inch_Per_Second2
    });

    UnitGroup elastic_modulus = UnitGroup("Elastic modulus", {
        Units::Gigapascal,
        Units::Megapascal,
        Units::Kilopascal,
        Units::Pascal,
        Units::Newton_Per_Millimeter2,
        Units::MegaPsi,
        Units::KiloPsi,
        Units::Psi
    });

    UnitGroup density = UnitGroup("Density", {
        Units::Kilogramm_Per_Meter3,
        Units::Gram_Per_Centimeter3,
        Units::Pound_Per_Foot3
    });

    UnitGroup linear_stiffness = UnitGroup("Linear stiffness", {
        Units::Newton,
        Units::Newton_Per_Percent,
        Units::PoundForce,
        Units::PoundForce_Per_Percent
    });

    UnitGroup linear_density = UnitGroup("Linear density", {
        Units::Kilogramm_Per_Meter,
        Units::Gram_Per_Meter,
        Units::Gram_Per_Centimeter,
        Units::Pound_Per_Foot,
        Units::Pound_Per_Inch,
        Units::Ounce_Per_Foot,
        Units::Ounce_Per_Inch
    });

    UnitGroup strain = UnitGroup("Strain", {
        Units::No_Unit,
        Units::Percent
    });

    UnitGroup curvature = UnitGroup("Curvature", {
        Units::One_Per_Meter,
        Units::One_Per_Centimeter,
        Units::One_Per_Millimeter,
        Units::One_Per_Foot,
        Units::One_Per_Inch
    });

    UnitGroup stress = UnitGroup("Stress", {
        Units::Gigapascal,
        Units::Megapascal,
        Units::Kilopascal,
        Units::Pascal,
        Units::Newton_Per_Millimeter2,
        Units::MegaPsi,
        Units::KiloPsi,
        Units::Psi
    });

    UnitGroup ratio = UnitGroup("Ratios and factors", {
        Units::No_Unit,
        Units::Percent
    });

    UnitGroup frequency = UnitGroup("Frequency", {
        Units::Hertz,
        Units::Kilohertz,
        Units::Megahertz
    });
};
