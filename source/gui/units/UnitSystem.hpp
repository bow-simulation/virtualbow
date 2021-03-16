#pragma once
#include "Definitions.hpp"
#include <QtCore>
#include <cmath>

struct UnitSystem {

    Unit length;
    Unit angle;
    Unit mass;
    Unit force;
    Unit time;
    Unit energy;
    Unit position;
    Unit velocity;
    Unit acceleration;
    Unit elastic_modulus;
    Unit density;
    Unit linear_stiffness;
    Unit linear_density;
    Unit strain;
    Unit curvature;
    Unit stress;
    Unit damping_ratio;
    Unit relative_position;

    static void saveToSettings(const UnitSystem& units) {
        QSettings settings;

        auto save_unit = [&](const QString id, const Unit& unit) {
            settings.setValue("Units/" + id, QVariant(unit.getName()));
        };

        save_unit("length", units.length);
        save_unit("angle", units.angle);
        save_unit("mass", units.mass);
        save_unit("force", units.force);
        save_unit("time", units.time);
        save_unit("energy", units.energy);
        save_unit("position", units.position);
        save_unit("velocity", units.velocity);
        save_unit("acceleration", units.acceleration);
        save_unit("elastic_modulus", units.elastic_modulus);
        save_unit("density", units.density);
        save_unit("linear_stiffness", units.linear_stiffness);
        save_unit("linear_density", units.linear_density);
        save_unit("strain", units.strain);
        save_unit("curvature", units.curvature);
        save_unit("stress", units.stress);
        save_unit("damping_ratio", units.damping_ratio);
        save_unit("relative_position", units.relative_position);
    }

    static UnitSystem loadFromSettings() {
        QSettings settings;

        auto load_unit = [&](const QString& name, const QList<Unit>& options, const Unit& default_unit){
            QString unit_name = settings.value("Units/" + name).toString();
            for(auto& unit: options) {
                if(unit.getName() == unit_name) {
                    return unit;
                }
            }
            return default_unit;
        };

        return {
            .length = load_unit("length", Units::Length, Units::Millimeter),
            .angle = load_unit("angle", Units::Angle, Units::Degree),
            .mass = load_unit("mass", Units::Mass, Units::Kilogram),
            .force = load_unit("force", Units::Force, Units::Newton),
            .time = load_unit("time", Units::Time, Units::Second),
            .energy = load_unit("energy", Units::Energy, Units::Joule),
            .position = load_unit("position", Units::Position, Units::Meter),
            .velocity = load_unit("velocity", Units::Velocity, Units::Meter_Per_Second),
            .acceleration = load_unit("acceleration", Units::Acceleration, Units::Meter_Per_Second2),
            .elastic_modulus = load_unit("elastic_modulus", Units::Elastic_Modulus, Units::Gigapascal),
            .density = load_unit("density", Units::Density, Units::Kilogramm_Per_Meter3),
            .linear_stiffness = load_unit("linear_stiffness", Units::Linear_Stiffness, Units::Newton_Per_Percent),
            .linear_density = load_unit("linear_density", Units::Linear_Density, Units::Gram_Per_Meter),
            .strain = load_unit("strain", Units::Strain, Units::Percent),
            .curvature = load_unit("curvature", Units::Curvature, Units::One_Per_Millimeter),
            .stress = load_unit("stress", Units::Stress, Units::Megapascal),
            .damping_ratio = load_unit("damping_ratio", Units::Damping_Ratio, Units::Percent),
            .relative_position = load_unit("relative_position", Units::Relative_Position, Units::Percent)
        };
    }
};
