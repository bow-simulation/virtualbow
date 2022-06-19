#pragma once
#include "Definitions.hpp"
#include "UnitGroup.hpp"

class UnitSystem {
public:
    static UnitGroup length;
    static UnitGroup angle;
    static UnitGroup mass;
    static UnitGroup force;
    static UnitGroup time;
    static UnitGroup energy;
    static UnitGroup position;
    static UnitGroup velocity;
    static UnitGroup acceleration;
    static UnitGroup elastic_modulus;
    static UnitGroup density;
    static UnitGroup linear_stiffness;
    static UnitGroup linear_density;
    static UnitGroup strain;
    static UnitGroup curvature;
    static UnitGroup stress;
    static UnitGroup ratio;
    static UnitGroup frequency;
    static UnitGroup none;

    static QVector<UnitGroup*> groups;

    static void loadFromSettings(const QSettings& settings);
    static void saveToSettings(QSettings& settings);
    static void resetSI();
    static void resetUS();
};
