#include "Unit.hpp"

Unit::Unit(const QString& shortName, const QString& fullName, UnitType type, double factor):
    shortName(shortName),
    fullName(fullName),
    type(type),
    factor(factor)
{

}

bool Unit::operator==(const Unit& other) const {
    return (shortName == other.shortName) && (type == other.type) && (factor == other.factor);
}

bool Unit::operator!=(const Unit& other) const {
    return !operator==(other);
}

double Unit::fromBase(double value) const {
    return value/factor;
}

double Unit::toBase(double value) const {
    return value*factor;
}

QVector<double> Unit::fromBase(const std::vector<double>& value) const {
    QVector<double> result(value.size());
    for(size_t i = 0; i < value.size(); ++i) {
        result[i] = fromBase(value[i]);
    }
    return result;
}

QVector<double> Unit::toBase(const std::vector<double>& value) const {
    QVector<double> result(value.size());
    for(size_t i = 0; i < value.size(); ++i) {
        result[i] = toBase(value[i]);
    }
    return result;
}

UnitType Unit::getType() const {
    return type;
}

QString Unit::getSuffix() const {
    return shortName.isEmpty() ? "" : " " + shortName;
}

QString Unit::getSymbol() const {
    return shortName.isEmpty() ? "-" : shortName;
}

QString Unit::getName() const {
    return fullName;
}

QString Unit::getLabel() const {
    return "[ " + getSymbol() + " ]";
}
