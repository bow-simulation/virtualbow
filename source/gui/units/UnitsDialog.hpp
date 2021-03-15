#pragma once
#include "UnitSystem.hpp"
#include "gui/PersistentDialog.hpp"

class UnitEditor: public QWidget {
public:
    UnitEditor(const QString& text, const QList<Unit>& units);
    Unit getUnit() const;
    void setUnit(const Unit& unit);

private:
    const QList<Unit>& units;
    QComboBox* combo;
};

class UnitsDialog: public QDialog
{
public:
    UnitsDialog(QWidget* parent);
    UnitSystem getUnits() const;
    void setUnits(const UnitSystem& units);

private:
    UnitEditor* edit_length = new UnitEditor("Length", UnitOptions::LENGTH);
    UnitEditor* edit_angle = new UnitEditor("Angle", UnitOptions::ANGLE);
    UnitEditor* edit_mass = new UnitEditor("Mass", UnitOptions::MASS);
    UnitEditor* edit_force = new UnitEditor("Force", UnitOptions::FORCE);
    UnitEditor* edit_time = new UnitEditor("Time", UnitOptions::TIME);
    UnitEditor* edit_energy = new UnitEditor("Energy", UnitOptions::ENERGY);
    UnitEditor* edit_position = new UnitEditor("Position", UnitOptions::POSITION);
    UnitEditor* edit_velocity = new UnitEditor("Velocity", UnitOptions::VELOCITY);
    UnitEditor* edit_acceleration = new UnitEditor("Acceleration", UnitOptions::ACCELERATION);
    UnitEditor* edit_elastic_modulus = new UnitEditor("Elastic modulus", UnitOptions::ELASTIC_MODULUS);
    UnitEditor* edit_density = new UnitEditor("Density", UnitOptions::DENSITY);
    UnitEditor* edit_linear_stiffness = new UnitEditor("Linear stiffness", UnitOptions::LINEAR_STIFFNESS);
    UnitEditor* edit_linear_density = new UnitEditor("Linear density", UnitOptions::LINEAR_DENSITY);
    UnitEditor* edit_strain = new UnitEditor("Strain", UnitOptions::STRAIN);
    UnitEditor* edit_curvature = new UnitEditor("Curvature", UnitOptions::CURVATURE);
    UnitEditor* edit_stress = new UnitEditor("Stress", UnitOptions::STRESS);
    UnitEditor* edit_damping_ratio = new UnitEditor("Damping ratio", UnitOptions::DAMPING_RATIO);
    UnitEditor* edit_relative_position = new UnitEditor("Relative position", UnitOptions::RELATIVE_POSITION);
};
