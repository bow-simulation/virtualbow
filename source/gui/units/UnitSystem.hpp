#pragma once
#include "Definitions.hpp"
#include "UnitGroup.hpp"
#include <QtCore>
#include <cmath>

struct UnitSystem {
    UnitGroup length = UnitGroup("Length", {
        Units::Meter,
        Units::Centimeter,
        Units::Millimeter,
        Units::Foot,
        Units::Inch
    }, 2);

    UnitGroup angle = UnitGroup("Angle", {
        Units::Radian,
        Units::Degree
    }, 1);

    UnitGroup mass = UnitGroup("Mass", {
        Units::Kilogram,
        Units::Gram,
        Units::PoundMass,
        Units::Ounce,
        Units::Grain
    }, 1);

    UnitGroup force = UnitGroup("Force", {
        Units::Newton,
        Units::PoundForce
    }, 0);

    UnitGroup time = UnitGroup("Time", {
        Units::Second,
        Units::Millisecond
    }, 0);

    UnitGroup energy = UnitGroup("Energy",  {
        Units::Joule,
        Units::Foot_Pound
    }, 0);

    UnitGroup position = UnitGroup("Position", {
        Units::Meter,
        Units::Centimeter,
        Units::Millimeter,
        Units::Foot,
        Units::Inch
    }, 0);

    UnitGroup velocity = UnitGroup("Velocity", {
        Units::Meter_Per_Second,
        Units::Centimeter_Per_Second,
        Units::Millimeter_Per_Second,
        Units::Foot_Per_Second,
        Units::Inch_Per_Second
    }, 0);

    UnitGroup acceleration = UnitGroup("Acceleration", {
        Units::Meter_Per_Second2,
        Units::Centimeter_Per_Second2,
        Units::Millimeter_Per_Second2,
        Units::Foot_Per_Second2,
        Units::Inch_Per_Second2
    }, 0);

    UnitGroup elastic_modulus = UnitGroup("Elastic modulus", {
        Units::Gigapascal,
        Units::Megapascal,
        Units::Kilopascal,
        Units::Pascal,
        Units::Newton_Per_Millimeter2,
        Units::MegaPsi,
        Units::KiloPsi,
        Units::Psi
    }, 0);

    UnitGroup density = UnitGroup("Density", {
        Units::Kilogramm_Per_Meter3,
        Units::Gram_Per_Centimeter3,
        Units::Pound_Per_Foot3
    }, 0);

    UnitGroup linear_stiffness = UnitGroup("Linear stiffness", {
        Units::Newton,
        Units::Newton_Per_Percent,
        Units::PoundForce,
        Units::PoundForce_Per_Percent
    }, 0);

    UnitGroup linear_density = UnitGroup("Linear density", {
        Units::Kilogramm_Per_Meter,
        Units::Gram_Per_Meter,
        Units::Gram_Per_Centimeter,
        Units::Pound_Per_Foot,
        Units::Pound_Per_Inch,
        Units::Ounce_Per_Foot,
        Units::Ounce_Per_Inch
    }, 1);

    UnitGroup strain = UnitGroup("Strain", {
        Units::No_Unit,
        Units::Percent
    }, 1);

    UnitGroup curvature = UnitGroup("Curvature", {
        Units::One_Per_Meter,
        Units::One_Per_Centimeter,
        Units::One_Per_Millimeter,
        Units::One_Per_Foot,
        Units::One_Per_Inch
    }, 2);

    UnitGroup stress = UnitGroup("Stress", {
        Units::Gigapascal,
        Units::Megapascal,
        Units::Kilopascal,
        Units::Pascal,
        Units::Newton_Per_Millimeter2,
        Units::MegaPsi,
        Units::KiloPsi,
        Units::Psi
    }, 1);

    UnitGroup ratio = UnitGroup("Ratios and factors", {
        Units::No_Unit,
        Units::Percent
    }, 1);

    UnitGroup frequency = UnitGroup("Frequency", {
        Units::Hertz,
        Units::Kilohertz,
        Units::Megahertz
    }, 1);
};
