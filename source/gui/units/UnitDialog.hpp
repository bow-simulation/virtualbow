#pragma once
#include "UnitSystem.hpp"
#include "gui/PersistentDialog.hpp"

class UnitDialog;

class UnitEditor: public QWidget {
public:
    UnitEditor(UnitGroup& group);
};

class UnitDialog: public QDialog {
    Q_OBJECT

public:
    UnitDialog(QWidget* parent, UnitSystem& units);
};
