#pragma once
#include <QtCore>
#include <string>
#include <cmath>


class Unit {
public:
    Unit(const QString& name, double factor)
        : name(name), factor(factor) {
    }

    Unit(): Unit("", 1.0) {

    }

    double fromBase(double value) const {
        return value/factor;
    }

    double toBase(double value) const {
        return value*factor;
    }

    QString getName() const {
        return name;
    }

    QString getLabel() const {
        return name.isEmpty() ? QString() : "[" + name + "]";
    }

private:
    QString name;
    double factor;
};

struct UnitSystem {
    Unit none = Unit("1", 1.0);
    Unit length = Unit("mm", 1e-3);
    Unit angle = Unit("deg", M_PI/180.0);
    Unit mass = Unit("g", 1e-3);
    Unit force = Unit("N", 1.0);
    Unit time = Unit("s", 1.0);
    Unit energy = Unit("J", 1.0);
    Unit position = Unit("m", 1.0);
    Unit velocity = Unit("m/s", 1.0);
    Unit acceleration = Unit("m/s²", 1.0);
    Unit elastic_modulus = Unit("GPa", 1e9);
    Unit density = Unit("kg/m³", 1.0);
    Unit linear_stiffness = Unit("N", 1.0);
    Unit linear_density = Unit("g/m", 1e-3);
    Unit strain = Unit("%", 1e-2);
    Unit curvature = Unit("1/mm", 1e3);
    Unit stress = Unit("MPa", 1e6);
    Unit damping_ratio = Unit("%", 1e-2);
    Unit relative_position = Unit("%", 1e-2);
};
