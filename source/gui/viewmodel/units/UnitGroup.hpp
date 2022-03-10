#pragma once
#include "Unit.hpp"
#include <QtCore>

class UnitSystem;

class UnitGroup: public QObject {
    Q_OBJECT

public:
    UnitGroup() = default;
    UnitGroup(const QString& name, const QList<Unit>& units, int default_si, int default_us);

    void loadFromSettings(const QSettings& settings);
    void saveToSettings(QSettings& settings) const;

    const QString& getName() const;
    const QList<Unit>& getUnits() const;

    const Unit& getSelectedUnit() const;
    void setSelectedUnit(const Unit& unit);

    void setSelectedIndex(int index);
    int getSelectedIndex() const;

    void resetSI();
    void resetUS();

signals:
    void selectionChanged(const Unit& unit);

private:
    QString name;
    QList<Unit> units;

    int default_si;
    int default_us;
    int selection;

    QString getSettingsKey() const;
};
