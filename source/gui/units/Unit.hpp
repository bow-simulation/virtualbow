#pragma once
#include <QtCore>

class Unit {
public:
    enum class Type {
        SI, US
    };

    Unit(Type type, const QString& name, double factor)
        : type(type), name(name), factor(factor) {
    }

    Unit(): Unit(Type::SI, "", 1.0) {

    }

    double fromBase(double value) const {
        return value/factor;
    }

    double toBase(double value) const {
        return value*factor;
    }

    Type getType() const {
        return type;
    }

    QString getName() const {
        return name;
    }

    QString getLabel() const {
        return name.isEmpty() ? QString() : "[" + name + "]";
    }

private:
    Type type;
    QString name;
    double factor;
};
