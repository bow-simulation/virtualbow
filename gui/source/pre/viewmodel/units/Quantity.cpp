#include "Quantity.hpp"
#include "UnitSystem.hpp"

Quantity::Quantity(const QString& name, const QList<Unit>& units, int default_si, int default_us)
    : name(name),
      units(units),
      default_si(default_si),
      default_us(default_us),
      selection(default_si)
{ 
    if(units.isEmpty()) {
        throw std::invalid_argument("Unit list must not be empty");
    }
}

void Quantity::loadFromSettings(const QSettings& settings) {
    QVariant nameSetting = settings.value(getSettingsKey());
    if(nameSetting.isValid()) {
        for(int index = 0; index < units.size(); ++index) {
            if(units[index].getName() == nameSetting.toString()) {
                setSelectedIndex(index);
            }
        }
    }
}

void Quantity::saveToSettings(QSettings& settings) const {
    settings.setValue(getSettingsKey(), getUnit().getName());
}

QString Quantity::getSettingsKey() const {
    return "Units/" + name.toLower().replace(" ", "_");
}

const QString& Quantity::getName() const {
    return name;
}

const QList<Unit>& Quantity::getUnits() const {
    return units;
}

const Unit& Quantity::getUnit() const {
    return units[selection];
}

void Quantity::setUnit(const Unit& unit) {
    for(int index = 0; index < units.length(); ++index) {
        if(units[index] == unit) {
            setSelectedIndex(index);
            return;
        }
    }

    throw std::invalid_argument("Unit not in list");
}

void Quantity::setSelectedIndex(int index) {
    if(index != selection) {
        selection = index;
        emit unitChanged(getUnit());
    }
}

int Quantity::getSelectedIndex() const {
    return selection;
}

void Quantity::resetSI() {
    setSelectedIndex(default_si);
}

void Quantity::resetUS() {
    setSelectedIndex(default_us);
}
