#pragma once
#include "Unit.hpp"
#include <QtCore>
#include <cmath>

namespace UnitOptions {
    static const QList<Unit> LENGTH = {
        Unit(Unit::Type::SI, "m", 1.0),
        Unit(Unit::Type::SI, "cm", 1e-2),
        Unit(Unit::Type::SI, "mm", 1e-3),
        Unit(Unit::Type::US, "ft", 0.3048),
        Unit(Unit::Type::US, "in", 0.0254)
    };

    static const QList<Unit> ANGLE = {
        Unit(Unit::Type::SI, "rad", 1.0),
        Unit(Unit::Type::SI, "deg", M_PI/180.0)
    };

    static const QList<Unit> MASS = {
        Unit(Unit::Type::SI, "kg", 0.0),
        Unit(Unit::Type::SI, "g", 0.0),
        Unit(Unit::Type::US, "lb", 0.0),
        Unit(Unit::Type::US, "oz", 0.0),
        Unit(Unit::Type::US, "gr", 0.0)
    };

    static const QList<Unit> FORCE = {
        Unit(Unit::Type::SI, "N", 1e0),
        Unit(Unit::Type::US, "lb", 0.0)
    };

    static const QList<Unit> TIME = {
        Unit(Unit::Type::SI, "s", 1e0),
        Unit(Unit::Type::SI, "ms", 1e-3)
    };

    static const QList<Unit> ENERGY = {
        Unit(Unit::Type::SI, "J", 1e0),
        Unit(Unit::Type::US, "ft*lb", 0.0)
    };

    static const QList<Unit> POSITION = {
        Unit(Unit::Type::SI, "m", 1e0),
        Unit(Unit::Type::SI, "cm", 1e-2),
        Unit(Unit::Type::SI, "mm", 1e-3),
        Unit(Unit::Type::US, "ft", 0.0),
        Unit(Unit::Type::US, "in", 0.0)
    };

    static const QList<Unit> VELOCITY = {
        Unit(Unit::Type::SI, "m/s", 1e0),
        Unit(Unit::Type::SI, "cm/s", 1e-2),
        Unit(Unit::Type::SI, "mm/s", 1e-3),
        Unit(Unit::Type::US, "ft/s", 0.0),
        Unit(Unit::Type::US, "in/s", 0.0)
    };

    static const QList<Unit> ACCELERATION = {
        Unit(Unit::Type::SI, "m/s²", 1e0),
        Unit(Unit::Type::SI, "cm/s²", 1e-2),
        Unit(Unit::Type::SI, "mm/s²", 1e-3),
        Unit(Unit::Type::US, "ft/s²", 0.0),
        Unit(Unit::Type::US, "in/s²", 0.0)
    };

    static const QList<Unit> ELASTIC_MODULUS = {
        Unit(Unit::Type::SI, "GPa", 1e9),
        Unit(Unit::Type::SI, "MPa", 1e6),
        Unit(Unit::Type::SI, "KPa", 1e3),
        Unit(Unit::Type::SI, "Pa", 1e0),
        Unit(Unit::Type::SI, "N/mm²", 1e6),
        Unit(Unit::Type::US, "Mpsi", 0),
        Unit(Unit::Type::US, "Kpsi", 0),
        Unit(Unit::Type::US, "psi", 0)
    };

    static const QList<Unit> DENSITY = {
        Unit(Unit::Type::SI, "kg/m³", 1e0),
        Unit(Unit::Type::SI, "g/cm³", 0),
        Unit(Unit::Type::US, "lb/ft³", 0)
    };

    static const QList<Unit> LINEAR_STIFFNESS = {
        Unit(Unit::Type::SI, "N", 1e0),
        Unit(Unit::Type::SI, "N/%", 0),
        Unit(Unit::Type::US, "lb", 0),
        Unit(Unit::Type::US, "lb/%", 0)
    };

    static const QList<Unit> LINEAR_DENSITY = {
        Unit(Unit::Type::SI, "kg/m", 1e0),
        Unit(Unit::Type::SI, "g/m", 0),
        Unit(Unit::Type::SI, "g/cm", 0),
        Unit(Unit::Type::US, "lb/yd", 0),
        Unit(Unit::Type::US, "lb/ft", 0),
        Unit(Unit::Type::US, "lb/in", 0),
    };

    static const QList<Unit> STRAIN = {
        Unit(Unit::Type::SI, "-", 1e0),
        Unit(Unit::Type::SI, "%", 1e-2)
    };

    static const QList<Unit> CURVATURE = {
        Unit(Unit::Type::SI, "1/m", 0),
        Unit(Unit::Type::SI, "1/cm", 0),
        Unit(Unit::Type::SI, "1/mm", 0),
        Unit(Unit::Type::US, "1/ft", 0),
        Unit(Unit::Type::US, "1/in", 0)
    };

    static const QList<Unit> STRESS = {
        Unit(Unit::Type::SI, "GPa", 0),
        Unit(Unit::Type::SI, "MPa", 0),
        Unit(Unit::Type::SI, "KPa", 0),
        Unit(Unit::Type::SI, "Pa", 0),
        Unit(Unit::Type::SI, "N/mm²", 0),
        Unit(Unit::Type::US, "lb/ft", 0),
        Unit(Unit::Type::US, "lb/in", 0),
        Unit(Unit::Type::US, "lb/yd", 0)
    };

    static const QList<Unit> DAMPING_RATIO = {
        Unit(Unit::Type::SI, "-", 1e0),
        Unit(Unit::Type::SI, "%", 1e-2)
    };

    static const QList<Unit> RELATIVE_POSITION = {
        Unit(Unit::Type::SI, "-", 1e0),
        Unit(Unit::Type::SI, "%", 1e-2)
    };
}

struct UnitSystem {
    Unit length = UnitOptions::LENGTH[2];
    Unit angle = UnitOptions::ANGLE[1];
    Unit mass = UnitOptions::MASS[1];
    Unit force = UnitOptions::FORCE[0];
    Unit time = UnitOptions::TIME[0];
    Unit energy = UnitOptions::ENERGY[0];
    Unit position = UnitOptions::POSITION[0];
    Unit velocity = UnitOptions::VELOCITY[0];
    Unit acceleration = UnitOptions::ACCELERATION[0];
    Unit elastic_modulus = UnitOptions::ELASTIC_MODULUS[0];
    Unit density = UnitOptions::DENSITY[0];
    Unit linear_stiffness = UnitOptions::LINEAR_STIFFNESS[0];
    Unit linear_density = UnitOptions::LINEAR_DENSITY[1];
    Unit strain = UnitOptions::STRAIN[1];
    Unit curvature = UnitOptions::CURVATURE[0];
    Unit stress = UnitOptions::STRESS[1];
    Unit damping_ratio = UnitOptions::DAMPING_RATIO[1];
    Unit relative_position = UnitOptions::RELATIVE_POSITION[1];
};
