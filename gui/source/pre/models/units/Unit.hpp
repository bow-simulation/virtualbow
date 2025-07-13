#pragma once
#include <QVector>
#include <QString>

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

    QVector<double> fromBase(const std::vector<double>& value) const;
    QVector<double> toBase(const std::vector<double>& value) const;

    UnitType getType() const;
    QString getName() const;
    QString getLabel() const;

private:
    QString name;
    UnitType type;
    double factor;
};
