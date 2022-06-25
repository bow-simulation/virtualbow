#pragma once
#include "Quantity.hpp"

class Units {
public:
    static const Unit Meter;
    static const Unit Centimeter;
    static const Unit Millimeter;
    static const Unit Foot;
    static const Unit Inch;

    static const Unit Radian;
    static const Unit Degree;

    static const Unit Kilogram;
    static const Unit Gram;
    static const Unit PoundMass;
    static const Unit Ounce;
    static const Unit Grain;

    static const Unit Newton;
    static const Unit Newton_Per_Percent;
    static const Unit PoundForce;
    static const Unit PoundForce_Per_Percent;

    static const Unit Second;
    static const Unit Millisecond;

    static const Unit Hertz;
    static const Unit Kilohertz;
    static const Unit Megahertz;

    static const Unit Joule;
    static const Unit Foot_Pound;

    static const Unit Meter_Per_Second;
    static const Unit Centimeter_Per_Second;
    static const Unit Millimeter_Per_Second;
    static const Unit Foot_Per_Second;
    static const Unit Inch_Per_Second;

    static const Unit Meter_Per_Second2;
    static const Unit Centimeter_Per_Second2;
    static const Unit Millimeter_Per_Second2;
    static const Unit Foot_Per_Second2;
    static const Unit Inch_Per_Second2;

    static const Unit Gigapascal;
    static const Unit Megapascal;
    static const Unit Kilopascal;
    static const Unit Pascal;
    static const Unit Newton_Per_Millimeter2;
    static const Unit MegaPsi;
    static const Unit KiloPsi;
    static const Unit Psi;

    static const Unit Kilogramm_Per_Meter3;
    static const Unit Gram_Per_Centimeter3;
    static const Unit Pound_Per_Foot3;

    static const Unit Kilogramm_Per_Meter;
    static const Unit Gram_Per_Meter;
    static const Unit Gram_Per_Centimeter;
    static const Unit Pound_Per_Foot;
    static const Unit Pound_Per_Inch;
    static const Unit Ounce_Per_Foot;
    static const Unit Ounce_Per_Inch;
    static const Unit Grain_Per_Foot;
    static const Unit Grain_Per_Inch;

    static const Unit One_Per_Meter;
    static const Unit One_Per_Centimeter;
    static const Unit One_Per_Millimeter;
    static const Unit One_Per_Foot;
    static const Unit One_Per_Inch;

    static const Unit No_Unit;
    static const Unit Percent;
};

class Quantities {
public:
    static Quantity length;
    static Quantity angle;
    static Quantity mass;
    static Quantity force;
    static Quantity time;
    static Quantity energy;
    static Quantity position;
    static Quantity velocity;
    static Quantity acceleration;
    static Quantity elastic_modulus;
    static Quantity density;
    static Quantity linear_stiffness;
    static Quantity linear_density;
    static Quantity strain;
    static Quantity curvature;
    static Quantity stress;
    static Quantity ratio;
    static Quantity frequency;
    static Quantity none;

    static QVector<Quantity*> quantities;

    static void loadFromSettings(const QSettings& settings);
    static void saveToSettings(QSettings& settings);
    static void resetSI();
    static void resetUS();
};
