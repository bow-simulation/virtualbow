#pragma once
#include "Unit.hpp"

namespace Units {
    const Unit Meter = Unit("m", SI, 1.0);
    const Unit Centimeter = Unit("cm", SI, 1e-2);
    const Unit Millimeter = Unit("mm", SI, 1e-3);
    const Unit Foot = Unit("ft", US, 0.3048);
    const Unit Inch = Unit("in", US, 0.0254);

    const Unit Radian = Unit("rad", SI, 1.0);
    const Unit Degree = Unit("deg", SI, M_PI/180.0);

    const Unit Kilogram = Unit("kg", SI, 1.0);
    const Unit Gram = Unit("g", SI, 1e-3);
    const Unit PoundMass = Unit("lb", US, 0.453592);
    const Unit Ounce = Unit("oz", US, 0.0283495);
    const Unit Grain = Unit("gr", US, 0.00006479891);

    const Unit Newton = Unit("N", SI, 1.0);
    const Unit Newton_Per_Percent = Unit("N/%", SI, 1e2);
    const Unit PoundForce = Unit("lb", US, 4.4482216153);
    const Unit PoundForce_Per_Percent = Unit("lb/%", US, 0.453592e2);

    const Unit Second = Unit("s", SI, 1.0);
    const Unit Millisecond = Unit("ms", SI, 1e-3);

    const Unit Hertz = Unit("Hz", SI, 1.0);
    const Unit Kilohertz = Unit("KHz", SI, 1e3);
    const Unit Megahertz = Unit("MHz", SI, 1e6);

    const Unit Joule = Unit("J", SI, 1.0);
    const Unit Foot_Pound = Unit("ft*lb", US, 1.35582);

    const Unit Meter_Per_Second = Unit("m/s", SI, 1.0);
    const Unit Centimeter_Per_Second = Unit("cm/s", SI, 1e-2);
    const Unit Millimeter_Per_Second = Unit("mm/s", SI, 1e-3);
    const Unit Foot_Per_Second = Unit("ft/s", US, 0.3048);
    const Unit Inch_Per_Second = Unit("in/s", US, 0.0254);

    const Unit Meter_Per_Second2 = Unit("m/s²", SI, 1.0);
    const Unit Centimeter_Per_Second2 = Unit("cm/s²", SI, 1e-2);
    const Unit Millimeter_Per_Second2 = Unit("mm/s²", SI, 1e-3);
    const Unit Foot_Per_Second2 = Unit("ft/s²", US, 0.3048);
    const Unit Inch_Per_Second2 = Unit("in/s²", US, 0.0254);

    const Unit Gigapascal = Unit("GPa", SI, 1e9);
    const Unit Megapascal = Unit("MPa", SI, 1e6);
    const Unit Kilopascal = Unit("KPa", SI, 1e3);
    const Unit Pascal = Unit("Pa", SI, 1e0);
    const Unit Newton_Per_Millimeter2 = Unit("N/mm²", SI, 1e6);
    const Unit MegaPsi = Unit("Mpsi", US, 6894.76e6);
    const Unit KiloPsi = Unit("Kpsi", US, 6894.76e3);
    const Unit Psi = Unit("psi", US, 6894.76);

    const Unit Kilogramm_Per_Meter3 = Unit("kg/m³", SI, 1.0);
    const Unit Gram_Per_Centimeter3 = Unit("g/cm³", SI, 1e3);
    const Unit Pound_Per_Foot3 = Unit("lb/ft³", US, 16.0185);

    const Unit Kilogramm_Per_Meter = Unit("kg/m", SI, 1.0);
    const Unit Gram_Per_Meter = Unit("g/m", SI, 1e-3);
    const Unit Gram_Per_Centimeter = Unit("g/cm", SI, 1e-1);
    const Unit Pound_Per_Foot = Unit("lb/ft", US, 1.48816);
    const Unit Pound_Per_Inch = Unit("lb/in", US, 0.0115212);
    const Unit Ounce_Per_Foot = Unit("oz/ft", US, 0.0930102);
    const Unit Ounce_Per_Inch = Unit("oz/in", US, 1.11612);
    const Unit Grain_Per_Foot = Unit("gr/ft", US, 0.000212595);
    const Unit Grain_Per_Inch = Unit("gr/in", US, 0.00255113819);

    const Unit One_Per_Meter = Unit("1/m", SI, 1.0);
    const Unit One_Per_Centimeter = Unit("1/cm", SI, 1e2);
    const Unit One_Per_Millimeter = Unit("1/mm", SI, 1e3);
    const Unit One_Per_Foot = Unit("1/ft", US, 3.28084);
    const Unit One_Per_Inch = Unit("1/in", US, 39.370079);

    const Unit No_Unit = Unit("", SI, 1.0);
    const Unit Percent = Unit("%", SI, 1e-2);
}
