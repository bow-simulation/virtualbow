#pragma once
#include "UnitSystem.hpp"
#include "gui/PersistentDialog.hpp"

class UnitEditor: public QWidget {
public:
    UnitEditor(const QString& text, Unit& binding, const QList<Unit>& units);

private:
    Unit& binding;
    const QList<Unit>& units;
};

class UnitsDialog: public QDialog
{
public:
    UnitsDialog(QWidget* parent, const UnitSystem& units);
    const UnitSystem& getUnits() const;

private:
    UnitSystem units;
};
