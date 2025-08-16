#pragma once
#include <QVector>
#include <QString>

enum UnitType {
    SI,
    US
};

class Unit {
public:
    Unit(const QString& shortName, const QString& fullName, UnitType type, double factor);
    bool operator==(const Unit& other) const;
    bool operator!=(const Unit& other) const;

    double fromBase(double value) const;
    double toBase(double value) const;

    QVector<double> fromBase(const std::vector<double>& value) const;
    QVector<double> toBase(const std::vector<double>& value) const;

    UnitType getType() const;
    QString getSuffix() const;    // Returns the unit suffix, i.e. the string to place after numbers including a single space. Can be empty.
    QString getSymbol() const;    // Returns the unit symbol which is like the suffix except that it won't be empty ("-" if no suffix)
    QString getName() const;      // Full name of the unit in words
    QString getLabel() const;     // Unit symbol in brackets

private:
    QString shortName;
    QString fullName;
    UnitType type;
    double factor;
};
