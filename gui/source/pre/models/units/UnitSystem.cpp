#include "UnitSystem.hpp"

const Unit Units::Meter = Unit("m", "Meter", SI, 1.0);
const Unit Units::Centimeter = Unit("cm", "Centimeter", SI, 1e-2);
const Unit Units::Millimeter = Unit("mm", "Millimeter", SI, 1e-3);
const Unit Units::Foot = Unit("ft", "Foot", US, 0.3048);
const Unit Units::Inch = Unit("in", "Inch", US, 0.0254);

const Unit Units::Radian = Unit("rad", "Radian", SI, 1.0);
const Unit Units::Degree = Unit("deg", "Degree", SI, M_PI/180.0);

const Unit Units::Kilogram = Unit("kg", "Kilogram", SI, 1.0);
const Unit Units::Gram = Unit("g", "Gram", SI, 1e-3);
const Unit Units::PoundMass = Unit("lb", "Pound", US, 0.453592);
const Unit Units::Ounce = Unit("oz", "Ounce", US, 0.0283495);
const Unit Units::Grain = Unit("gr", "Grain", US, 0.00006479891);

const Unit Units::Kilogram_Per_Newton = Unit("kg/N", "Kilogram per Newton", SI, 1.0);
const Unit Units::Gram_Per_Newton = Unit("g/N", "Gram per Newton", SI, 1e-3);
const Unit Units::Pound_Per_PoundForce = Unit("lb/lbf", "Pound per pound-force", US, 0.453592/4.44822);
const Unit Units::Ounce_Per_PoundForce = Unit("oz/lbf", "Ounce per pound-force", US, 0.0283495/4.44822);
const Unit Units::Grain_Per_PoundForce = Unit("gr/lbf", "Grain per pound-force", US, 0.00006479891/4.44822);

const Unit Units::Kilogram_Per_Joule = Unit("kg/J", "Kilogram per Joule", SI, 1.0);
const Unit Units::Gram_Per_Joule = Unit("g/J", "Gram per Joule", SI, 1e-3);
const Unit Units::Pound_Per_FootPound = Unit("lb/ft*lbf", "Pound per foot-pound", US, 0.453592/1.35582);
const Unit Units::Ounce_Per_FootPound = Unit("oz/ft*lbf", "Ounce per pound-force", US, 0.0283495/1.35582);
const Unit Units::Grain_Per_FootPound = Unit("gr/ft*lbf", "Grain per pound-force", US, 0.00006479891/1.35582);

const Unit Units::Newton = Unit("N", "Newton", SI, 1.0);
const Unit Units::Newton_Per_Percent = Unit("N/%", "Newton per percent", SI, 1e2);
const Unit Units::PoundForce = Unit("lbf", "Pound-force", US, 4.4482216153);
const Unit Units::PoundForce_Per_Percent = Unit("lbf/%", "Pound-force per percent", US, 0.453592e2);

const Unit Units::Newton_Per_Meter = Unit("N/m", "Newton per meter", SI, 1.0);
const Unit Units::Newton_Per_Centimeter = Unit("N/cm", "Newton per centimeter", SI, 1e2);
const Unit Units::Newton_Per_Millimeter = Unit("N/mm", "Newton per millimeter", SI, 1e3);
const Unit Units::PoundForce_Per_Inch = Unit("lbf/in", "Pound-force per inch", US, 4.4482216153/0.0254);

const Unit Units::Second = Unit("s", "Second", SI, 1.0);
const Unit Units::Millisecond = Unit("ms", "Millisecond", SI, 1e-3);

const Unit Units::Hertz = Unit("Hz", "Hertz", SI, 1.0);
const Unit Units::Kilohertz = Unit("kHz", "Kilohertz", SI, 1e3);
const Unit Units::Megahertz = Unit("MHz", "Megahertz", SI, 1e6);

const Unit Units::Joule = Unit("J", "Joule", SI, 1.0);
const Unit Units::Foot_Pound = Unit("ft*lbf", "Foot-pound", US, 1.35582);

const Unit Units::Meter_Per_Second = Unit("m/s", "Meter per second", SI, 1.0);
const Unit Units::Centimeter_Per_Second = Unit("cm/s", "Centimeter per second", SI, 1e-2);
const Unit Units::Millimeter_Per_Second = Unit("mm/s", "Millimeter per second", SI, 1e-3);
const Unit Units::Foot_Per_Second = Unit("ft/s", "Foot per second", US, 0.3048);
const Unit Units::Inch_Per_Second = Unit("in/s", "Inch per second", US, 0.0254);

const Unit Units::Meter_Per_Second2 = Unit("m/s²", "Meter per second squared", SI, 1.0);
const Unit Units::Centimeter_Per_Second2 = Unit("cm/s²", "Centimeter per second squared", SI, 1e-2);
const Unit Units::Millimeter_Per_Second2 = Unit("mm/s²", "Millimeter per second squared", SI, 1e-3);
const Unit Units::Foot_Per_Second2 = Unit("ft/s²", "Foot per second squared", US, 0.3048);
const Unit Units::Inch_Per_Second2 = Unit("in/s²", "Inch per second squared", US, 0.0254);

const Unit Units::Gigapascal = Unit("GPa", "Gigapascal", SI, 1e9);
const Unit Units::Megapascal = Unit("MPa", "Megapascal", SI, 1e6);
const Unit Units::Kilopascal = Unit("kPa", "Kilopascal", SI, 1e3);
const Unit Units::Pascal = Unit("Pa", "Pascal", SI, 1e0);
const Unit Units::Newton_Per_Millimeter2 = Unit("N/mm²", "Newton per square millimeter", SI, 1e6);
const Unit Units::MegaPsi = Unit("Mpsi", "Megapound per square inch", US, 6894.76e6);
const Unit Units::KiloPsi = Unit("Kpsi", "Kilopound per square inch", US, 6894.76e3);
const Unit Units::Psi = Unit("psi", "Pound per square inch", US, 6894.76);

const Unit Units::Kilogramm_Per_Meter3 = Unit("kg/m³", "Kilogram per cubic meter", SI, 1.0);
const Unit Units::Gram_Per_Centimeter3 = Unit("g/cm³", "Gram per cubic centimeter", SI, 1e3);
const Unit Units::Pound_Per_Foot3 = Unit("lb/ft³", "Pound per cubic foot", US, 16.0185);

const Unit Units::Kilogramm_Per_Meter = Unit("kg/m", "Kilogram per meter", SI, 1.0);
const Unit Units::Gram_Per_Meter = Unit("g/m", "Gram per meter", SI, 1e-3);
const Unit Units::Gram_Per_Centimeter = Unit("g/cm", "Gram per centimeter", SI, 1e-1);
const Unit Units::Pound_Per_Foot = Unit("lb/ft", "Pound per foot", US, 1.48816);
const Unit Units::Pound_Per_Inch = Unit("lb/in", "Pound per inch", US, 0.0115212);
const Unit Units::Ounce_Per_Foot = Unit("oz/ft", "Ounce per foot", US, 0.0930102);
const Unit Units::Ounce_Per_Inch = Unit("oz/in", "Ounce per inch", US, 1.11612);
const Unit Units::Grain_Per_Foot = Unit("gr/ft", "Grain per foot", US, 0.000212595);
const Unit Units::Grain_Per_Inch = Unit("gr/in", "Grain per inch", US, 0.00255113819);

const Unit Units::One_Per_Meter = Unit("1/m", "One per meter", SI, 1.0);
const Unit Units::One_Per_Centimeter = Unit("1/cm", "One per centimeter", SI, 1e2);
const Unit Units::One_Per_Millimeter = Unit("1/mm", "One per millimeter", SI, 1e3);
const Unit Units::One_Per_Foot = Unit("1/ft", "One per foot", US, 3.28084);
const Unit Units::One_Per_Inch = Unit("1/in", "One per inch", US, 39.370079);

const Unit Units::No_Unit = Unit("", "No unit", SI, 1.0);
const Unit Units::Percent = Unit("%", "Percent", SI, 1e-2);

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

Quantity Quantities::mass_per_force = Quantity("Mass per Force", {
    Units::Kilogram_Per_Newton,
    Units::Gram_Per_Newton,
    Units::Pound_Per_PoundForce,
    Units::Ounce_Per_PoundForce,
    Units::Grain_Per_PoundForce,
}, 1, 4);

Quantity Quantities::mass_per_energy = Quantity("Mass per Energy", {
    Units::Kilogram_Per_Joule,
    Units::Gram_Per_Joule,
    Units::Pound_Per_FootPound,
    Units::Ounce_Per_FootPound,
    Units::Grain_Per_FootPound,
}, 1, 4);

Quantity Quantities::force = Quantity("Force", {
    Units::Newton,
    Units::PoundForce
}, 0, 1);

Quantity Quantities::stiffness = Quantity("Stiffness", {
    Units::Newton_Per_Meter,
    Units::Newton_Per_Centimeter,
    Units::Newton_Per_Millimeter,
    Units::PoundForce_Per_Inch,
}, 2, 3);

Quantity Quantities::time = Quantity("Time", {
    Units::Second,
    Units::Millisecond
}, 1, 1);

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
    &mass_per_force,
    &mass_per_energy,
    &force,
    &stiffness,
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
