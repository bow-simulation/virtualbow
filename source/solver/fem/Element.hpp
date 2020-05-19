#pragma once

class System;

class Element
{
public:
    Element(System& system);
    virtual ~Element();

    virtual void add_masses() const = 0;
    virtual void add_internal_forces() const = 0;
    virtual void add_tangent_stiffness() const = 0;
    virtual void add_tangent_damping() const = 0;

    virtual double get_potential_energy() const = 0;
    virtual double get_kinetic_energy() const = 0;

protected:
    System* system;
};
