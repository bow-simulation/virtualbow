#include "Unit.hpp"

Unit::Unit(const QString& name, const QString& text, UnitType type, double factor):
    label(name),
    text(text),
    type(type),
    factor(factor)
{

}

bool Unit::operator==(const Unit& other) const {
    return (label == other.label) && (type == other.type) && (factor == other.factor);
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

QString Unit::getLabel() const {
    return label;
}

QString Unit::getText() const {
    return text;
}

QString Unit::getSuffix() const {
    return label.isEmpty() ? QString() : "[ " + label + " ]";
}
