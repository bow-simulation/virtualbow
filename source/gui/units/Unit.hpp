#pragma once
#include <QtCore>

enum Type {
    SI,
    US
};

class Unit {
public:
    Unit(const QString& name, Type type, double factor);
    Unit();

    double fromBase(double value) const;
    double toBase(double value) const;

    Type getType() const;
    QString getName() const;
    QString getLabel() const;

private:
    QString name;
    Type type;
    double factor;
};
