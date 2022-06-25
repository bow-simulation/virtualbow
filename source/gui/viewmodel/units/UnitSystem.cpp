#include "UnitSystem.hpp"

const Unit Units::Meter = Unit("m", SI, 1.0);
const Unit Units::Centimeter = Unit("cm", SI, 1e-2);
const Unit Units::Millimeter = Unit("mm", SI, 1e-3);
const Unit Units::Foot = Unit("ft", US, 0.3048);
const Unit Units::Inch = Unit("in", US, 0.0254);

const Unit Units::Radian = Unit("rad", SI, 1.0);
const Unit Units::Degree = Unit("deg", SI, M_PI/180.0);

const Unit Units::Kilogram = Unit("kg", SI, 1.0);
const Unit Units::Gram = Unit("g", SI, 1e-3);
const Unit Units::PoundMass = Unit("lb", US, 0.453592);
const Unit Units::Ounce = Unit("oz", US, 0.0283495);
const Unit Units::Grain = Unit("gr", US, 0.00006479891);

const Unit Units::Newton = Unit("N", SI, 1.0);
const Unit Units::Newton_Per_Percent = Unit("N/%", SI, 1e2);
const Unit Units::PoundForce = Unit("lb", US, 4.4482216153);
const Unit Units::PoundForce_Per_Percent = Unit("lb/%", US, 0.453592e2);

const Unit Units::Second = Unit("s", SI, 1.0);
const Unit Units::Millisecond = Unit("ms", SI, 1e-3);

const Unit Units::Hertz = Unit("Hz", SI, 1.0);
const Unit Units::Kilohertz = Unit("kHz", SI, 1e3);
const Unit Units::Megahertz = Unit("MHz", SI, 1e6);

const Unit Units::Joule = Unit("J", SI, 1.0);
const Unit Units::Foot_Pound = Unit("ft*lb", US, 1.35582);

const Unit Units::Meter_Per_Second = Unit("m/s", SI, 1.0);
const Unit Units::Centimeter_Per_Second = Unit("cm/s", SI, 1e-2);
const Unit Units::Millimeter_Per_Second = Unit("mm/s", SI, 1e-3);
const Unit Units::Foot_Per_Second = Unit("ft/s", US, 0.3048);
const Unit Units::Inch_Per_Second = Unit("in/s", US, 0.0254);

const Unit Units::Meter_Per_Second2 = Unit("m/s²", SI, 1.0);
const Unit Units::Centimeter_Per_Second2 = Unit("cm/s²", SI, 1e-2);
const Unit Units::Millimeter_Per_Second2 = Unit("mm/s²", SI, 1e-3);
const Unit Units::Foot_Per_Second2 = Unit("ft/s²", US, 0.3048);
const Unit Units::Inch_Per_Second2 = Unit("in/s²", US, 0.0254);

const Unit Units::Gigapascal = Unit("GPa", SI, 1e9);
const Unit Units::Megapascal = Unit("MPa", SI, 1e6);
const Unit Units::Kilopascal = Unit("kPa", SI, 1e3);
const Unit Units::Pascal = Unit("Pa", SI, 1e0);
const Unit Units::Newton_Per_Millimeter2 = Unit("N/mm²", SI, 1e6);
const Unit Units::MegaPsi = Unit("Mpsi", US, 6894.76e6);
const Unit Units::KiloPsi = Unit("Kpsi", US, 6894.76e3);
const Unit Units::Psi = Unit("psi", US, 6894.76);

const Unit Units::Kilogramm_Per_Meter3 = Unit("kg/m³", SI, 1.0);
const Unit Units::Gram_Per_Centimeter3 = Unit("g/cm³", SI, 1e3);
const Unit Units::Pound_Per_Foot3 = Unit("lb/ft³", US, 16.0185);

const Unit Units::Kilogramm_Per_Meter = Unit("kg/m", SI, 1.0);
const Unit Units::Gram_Per_Meter = Unit("g/m", SI, 1e-3);
const Unit Units::Gram_Per_Centimeter = Unit("g/cm", SI, 1e-1);
const Unit Units::Pound_Per_Foot = Unit("lb/ft", US, 1.48816);
const Unit Units::Pound_Per_Inch = Unit("lb/in", US, 0.0115212);
const Unit Units::Ounce_Per_Foot = Unit("oz/ft", US, 0.0930102);
const Unit Units::Ounce_Per_Inch = Unit("oz/in", US, 1.11612);
const Unit Units::Grain_Per_Foot = Unit("gr/ft", US, 0.000212595);
const Unit Units::Grain_Per_Inch = Unit("gr/in", US, 0.00255113819);

const Unit Units::One_Per_Meter = Unit("1/m", SI, 1.0);
const Unit Units::One_Per_Centimeter = Unit("1/cm", SI, 1e2);
const Unit Units::One_Per_Millimeter = Unit("1/mm", SI, 1e3);
const Unit Units::One_Per_Foot = Unit("1/ft", US, 3.28084);
const Unit Units::One_Per_Inch = Unit("1/in", US, 39.370079);

const Unit Units::No_Unit = Unit("-", SI, 1.0);
const Unit Units::Percent = Unit("%", SI, 1e-2);

Quantity Quantities::length = Quantity("Length", {
    Units::Meter,
    Units::Centimeter,
    Units::Millimeter,
    Units::Foot,
    Units::Inch
}, 2, 4);

Quantity Quantities::angle = Quantity("Angle", {
    Units::Radian,
    Units::Degree
}, 1, 1);

Quantity Quantities::mass = Quantity("Mass", {
    Units::Kilogram,
    Units::Gram,
    Units::PoundMass,
    Units::Ounce,
    Units::Grain
}, 1, 4);

Quantity Quantities::force = Quantity("Force", {
    Units::Newton,
    Units::PoundForce
}, 0, 1);

Quantity Quantities::time = Quantity("Time", {
    Units::Second,
    Units::Millisecond
}, 0, 0);

Quantity Quantities::energy = Quantity("Energy",  {
    Units::Joule,
    Units::Foot_Pound
}, 0, 1);

Quantity Quantities::position = Quantity("Position", {
    Units::Meter,
    Units::Centimeter,
    Units::Millimeter,
    Units::Foot,
    Units::Inch
}, 0, 3);

Quantity Quantities::velocity = Quantity("Velocity", {
    Units::Meter_Per_Second,
    Units::Centimeter_Per_Second,
    Units::Millimeter_Per_Second,
    Units::Foot_Per_Second,
    Units::Inch_Per_Second
}, 0, 3);

Quantity Quantities::acceleration = Quantity("Acceleration", {
    Units::Meter_Per_Second2,
    Units::Centimeter_Per_Second2,
    Units::Millimeter_Per_Second2,
    Units::Foot_Per_Second2,
    Units::Inch_Per_Second2
}, 0, 3);

Quantity Quantities::elastic_modulus = Quantity("Elastic modulus", {
    Units::Gigapascal,
    Units::Megapascal,
    Units::Kilopascal,
    Units::Pascal,
    Units::Newton_Per_Millimeter2,
    Units::MegaPsi,
    Units::KiloPsi,
    Units::Psi
}, 0, 7);

Quantity Quantities::density = Quantity("Density", {
    Units::Kilogramm_Per_Meter3,
    Units::Gram_Per_Centimeter3,
    Units::Pound_Per_Foot3
}, 0, 2);

Quantity Quantities::linear_stiffness = Quantity("Linear stiffness", {
    Units::Newton,
    Units::Newton_Per_Percent,
    Units::PoundForce,
    Units::PoundForce_Per_Percent
}, 0, 2);

Quantity Quantities::linear_density = Quantity("Linear density", {
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

Quantity Quantities::strain = Quantity("Strain", {
    Units::No_Unit,
    Units::Percent
}, 1, 1);

Quantity Quantities::curvature = Quantity("Curvature", {
    Units::One_Per_Meter,
    Units::One_Per_Centimeter,
    Units::One_Per_Millimeter,
    Units::One_Per_Foot,
    Units::One_Per_Inch
}, 2, 4);

Quantity Quantities::stress = Quantity("Stress", {
    Units::Gigapascal,
    Units::Megapascal,
    Units::Kilopascal,
    Units::Pascal,
    Units::Newton_Per_Millimeter2,
    Units::MegaPsi,
    Units::KiloPsi,
    Units::Psi
}, 1, 7);

Quantity Quantities::ratio = Quantity("Ratios and factors", {
    Units::No_Unit,
    Units::Percent
}, 1, 1);

Quantity Quantities::frequency = Quantity("Frequency", {
    Units::Hertz,
    Units::Kilohertz,
    Units::Megahertz
}, 0, 0);

Quantity Quantities::none = Quantity("None", {
    Units::No_Unit
}, 0, 0);

QVector<Quantity*> Quantities::quantities = {
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
    &frequency,
    &none
};

void Quantities::loadFromSettings(const QSettings& settings) {
    for(auto quantity: quantities) {
        quantity->loadFromSettings(settings);
    }
}

void Quantities::saveToSettings(QSettings& settings) {
    for(auto quantity: quantities) {
        quantity->saveToSettings(settings);
    }
}

void Quantities::resetSI() {
    for(auto quantity: quantities) {
        quantity->resetSI();
    }
}

void Quantities::resetUS() {
    for(auto quantity: quantities) {
        quantity->resetUS();
    }
}
