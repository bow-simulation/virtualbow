#pragma once
#include "Unit.hpp"
#include <QtCore>

class Quantity: public QObject {
    Q_OBJECT

public:
    Quantity() = default;
    Quantity(const QString& name, const QList<Unit>& quantities, int default_si, int default_us);

    void loadFromSettings(const QSettings& settings);
    void saveToSettings(QSettings& settings) const;

    const QString& getName() const;
    const QList<Unit>& getUnits() const;

    const Unit& getUnit() const;
    void setUnit(const Unit& unit);

    void setSelectedIndex(int index);
    int getSelectedIndex() const;

    void resetSI();
    void resetUS();

signals:
    void unitChanged(const Unit& unit);

private:
    QString name;
    QList<Unit> units;

    int default_si;
    int default_us;
    int selection;

    QString getSettingsKey() const;
};
