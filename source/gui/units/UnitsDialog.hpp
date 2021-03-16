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

class UnitsDialog: public QDialog {
public:
    UnitsDialog(QWidget* parent);
    UnitSystem getUnits() const;
    void setUnits(const UnitSystem& units);

private:
    UnitEditor* edit_length = new UnitEditor("Length", Units::Length);
    UnitEditor* edit_angle = new UnitEditor("Angle", Units::Angle);
    UnitEditor* edit_mass = new UnitEditor("Mass", Units::Mass);
    UnitEditor* edit_force = new UnitEditor("Force", Units::Force);
    UnitEditor* edit_time = new UnitEditor("Time", Units::Time);
    UnitEditor* edit_energy = new UnitEditor("Energy", Units::Energy);

    UnitEditor* edit_position = new UnitEditor("Position", Units::Position);
    UnitEditor* edit_velocity = new UnitEditor("Velocity", Units::Velocity);
    UnitEditor* edit_acceleration = new UnitEditor("Acceleration", Units::Acceleration);
    UnitEditor* edit_elastic_modulus = new UnitEditor("Elastic modulus", Units::Elastic_Modulus);
    UnitEditor* edit_density = new UnitEditor("Density", Units::Density);
    UnitEditor* edit_linear_stiffness = new UnitEditor("Linear stiffness", Units::Linear_Stiffness);

    UnitEditor* edit_linear_density = new UnitEditor("Linear density", Units::Linear_Density);
    UnitEditor* edit_strain = new UnitEditor("Strain", Units::Strain);
    UnitEditor* edit_curvature = new UnitEditor("Curvature", Units::Curvature);
    UnitEditor* edit_stress = new UnitEditor("Stress", Units::Stress);
    UnitEditor* edit_damping_ratio = new UnitEditor("Damping ratio", Units::Damping_Ratio);
    UnitEditor* edit_relative_position = new UnitEditor("Relative position", Units::Relative_Position);
};
