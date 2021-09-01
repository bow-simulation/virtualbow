#pragma once
#include "gui/units/UnitSystem.hpp"
#include <QtWidgets>
#include <limits>

enum class Domain {
    UNRESTRICTED,
    POSITIVE,
    NEGATIVE,
    NON_POSITIVE,
    NON_NEGATIVE
};

class DoubleSpinBox: public QDoubleSpinBox {
public:
    DoubleSpinBox(Domain range, QWidget* parent = nullptr)
        : QDoubleSpinBox(parent),
          units(nullptr)
    {
        setDecimals(6);        // Magic number
        setSingleStep(0.1);    // Magic number

        switch(range) {
        case Domain::UNRESTRICTED:
            setMinimum(std::numeric_limits<double>::lowest());
            setMaximum(std::numeric_limits<double>::max());
            break;

        case Domain::POSITIVE:
            setMinimum(std::numeric_limits<double>::min());
            setMaximum(std::numeric_limits<double>::max());
            break;

        case Domain::NEGATIVE:
            setMinimum(std::numeric_limits<double>::lowest());
            setMaximum(-std::numeric_limits<double>::min());
            break;

        case Domain::NON_POSITIVE:
            setMinimum(std::numeric_limits<double>::lowest());
            setMaximum(0.0);
            break;

        case Domain::NON_NEGATIVE:
            setMinimum(0.0);
            setMaximum(std::numeric_limits<double>::max());
            break;
        }
    }

    void setUnitGroup(const UnitGroup* new_units) {
        if(units != nullptr) {
            QObject::disconnect(units, &UnitGroup::selectionChanged, this, &DoubleSpinBox::updateUnit);
        }
        if(new_units != nullptr) {
            QObject::connect(new_units, &UnitGroup::selectionChanged, this, &DoubleSpinBox::updateUnit);
        }
        units = new_units;
        updateUnit();
    }

    QString textFromValue(double value) const override {
        if(units != nullptr) {
            return QString::number(units->getSelectedUnit().fromBase(value));
        }
        else {
            return QString::number(value);
        }
    }

    double valueFromText(const QString& text) const override {
        QString number = text;
        number.remove(suffix());
        double value = QLocale().toDouble(number);

        if(units != nullptr) {
            return units->getSelectedUnit().toBase(value);
        }
        else {
            return value;
        }
    }

private:
    const UnitGroup* units;

    void updateUnit() {
        setSuffix(" " + units->getSelectedUnit().getName());    // This also triggers a new evaluation of textFromValue
        setSingleStep(units->getSelectedUnit().toBase(0.1));    // Magic number
    }
};
