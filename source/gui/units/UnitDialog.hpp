#pragma once
#include "UnitSystem.hpp"
#include "gui/PersistentDialog.hpp"

class UnitEditor: public QWidget {
public:
    UnitEditor(UnitGroup& group);
};

class UnitDialog: public QDialog {
public:
    UnitDialog(QWidget* parent, UnitSystem& units);
};
