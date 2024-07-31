#pragma once
#include "solver/numerics/EigenTypes.hpp"
#include <QtCore>

enum UnitType {
    SI,
    US
};

class Unit {
public:
    Unit(const QString& name, UnitType type, double factor);
    bool operator==(const Unit& other) const;
    bool operator!=(const Unit& other) const;

    double fromBase(double value) const;
    double toBase(double value) const;

    VectorXd fromBase(const VectorXd& value) const;    
    VectorXd toBase(const VectorXd& value) const;

    std::vector<double> fromBase(const std::vector<double>& value) const;
    std::vector<double> toBase(const std::vector<double>& value) const;

    UnitType getType() const;
    QString getName() const;
    QString getLabel() const;

private:
    QString name;
    UnitType type;
    double factor;
};
