#include "UnitGroup.hpp"
#include "UnitSystem.hpp"

UnitGroup::UnitGroup(const QString& name, const QList<Unit>& units, int default_si, int default_us)
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

void UnitGroup::loadFromSettings(const QSettings& settings) {
    QVariant nameSetting = settings.value(getSettingsKey());
    if(nameSetting.isValid()) {
        for(int index = 0; index < units.size(); ++index) {
            if(units[index].getName() == nameSetting.toString()) {
                setSelectedIndex(index);
            }
        }
    }
}

void UnitGroup::saveToSettings(QSettings& settings) const {
    settings.setValue(getSettingsKey(), getSelectedUnit().getName());
}

QString UnitGroup::getSettingsKey() const {
    return "Units/" + name.toLower().replace(" ", "_");
}

const QString& UnitGroup::getName() const {
    return name;
}

const QList<Unit>& UnitGroup::getUnits() const {
    return units;
}

const Unit& UnitGroup::getSelectedUnit() const {
    return units[selection];
}

void UnitGroup::setSelectedUnit(const Unit& unit) {
    for(int index = 0; index < units.length(); ++index) {
        if(units[index] == unit) {
            setSelectedIndex(index);
            return;
        }
    }

    throw std::invalid_argument("Unit not in list");
}

void UnitGroup::setSelectedIndex(int index) {
    if(index != selection) {
        selection = index;
        emit selectionChanged(getSelectedUnit());
    }
}

int UnitGroup::getSelectedIndex() const {
    return selection;
}

void UnitGroup::resetSI() {
    setSelectedIndex(default_si);
}

void UnitGroup::resetUS() {
    setSelectedIndex(default_us);
}
