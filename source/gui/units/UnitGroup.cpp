#include "UnitGroup.hpp"

UnitGroup::UnitGroup(const QString& name, const QList<Unit> units, int selection)
    : name(name),
      units(units),
      selection(selection)
{
    if(units.isEmpty()) {
        throw new std::invalid_argument("Unit list must not be empty");
    }

    // Load selection from settings
    QSettings settings;
    QVariant nameSetting = settings.value(getSettingsKey());
    if(!nameSetting.isValid()) {
        for(int index = 0; index < units.size(); ++index) {
            if(units[index].getName() == nameSetting.toString()) {
                selection = index;
            }
        }
    }
}

UnitGroup::~UnitGroup() {
    // Save selection to settings
    QSettings settings;
    settings.setValue(getSettingsKey(), getSelectedUnit().getName());
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

void UnitGroup::setSelectedIndex(int index) {
    selection = index;
    emit selectionChanged(getSelectedUnit());
}

int UnitGroup::getSelectedIndex() const {
    return selection;
}

QString UnitGroup::getSettingsKey() const {
    return "Units/" + name.toLower().replace(" ", "_");
}
