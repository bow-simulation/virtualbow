#include "Unit.hpp"

Unit::Unit(const QString& name, Type type, double factor)
    : name(name),
      type(type),
      factor(factor)
{

}

bool Unit::operator==(const Unit& other) {
    return (name == other.name) && (type == other.type) && (factor == other.factor);
}

double Unit::fromBase(double value) const {
    return value/factor;
}

double Unit::toBase(double value) const {
    return value*factor;
}

VectorXd Unit::fromBase(const VectorXd& value) const {
    return value/factor;
}

VectorXd Unit::toBase(const VectorXd& value) const {
    return value*factor;
}

std::vector<double> Unit::fromBase(const std::vector<double>& value) const {
    std::vector<double> result(value.size());
    for(size_t i = 0; i < value.size(); ++i) {
        result[i] = fromBase(value[i]);
    }
    return result;
}

std::vector<double> Unit::toBase(const std::vector<double>& value) const {
    std::vector<double> result(value.size());
    for(size_t i = 0; i < value.size(); ++i) {
        result[i] = toBase(value[i]);
    }
    return result;
}

Type Unit::getType() const {
    return type;
}

QString Unit::getName() const {
    return name;
}

QString Unit::getLabel() const {
    return name.isEmpty() ? QString() : "[ " + name + " ]";
}
