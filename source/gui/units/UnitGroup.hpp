#pragma once
#include "Unit.hpp"
#include <QtCore>

class UnitGroup: public QObject {
    Q_OBJECT

public:
    UnitGroup(const QString& name, const QList<Unit> units, int selection);
    ~UnitGroup();

    const QString& getName() const;
    const QList<Unit>& getUnits() const;
    const Unit& getSelectedUnit() const;

    void setSelectedIndex(int index);
    int getSelectedIndex() const;

signals:
    void selectionChanged(const Unit& unit);

private:
    QString name;
    QList<Unit> units;
    int selection;

    QString getSettingsKey() const;
};
