#include "Unit.hpp"

Unit::Unit(const QString& name, Type type, double factor)
    : name(name),
      type(type),
      factor(factor)
{

}

Unit::Unit()
    : Unit("", SI, 1.0)
{

}

double Unit::fromBase(double value) const {
    return value/factor;
}

double Unit::toBase(double value) const {
    return value*factor;
}

Type Unit::getType() const {
    return type;
}

QString Unit::getName() const {
    return name;
}

QString Unit::getLabel() const {
    return name.isEmpty() ? QString() : "[" + name + "]";
}
