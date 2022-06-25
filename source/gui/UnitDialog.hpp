#pragma once
#include "gui/viewmodel/units/UnitSystem.hpp"
#include "gui/widgets/PersistentDialog.hpp"

class UnitDialog;

class UnitEditor: public QWidget {
public:
    UnitEditor(Quantity& group);
};

class UnitDialog: public QDialog {
    Q_OBJECT
public:
    UnitDialog(QWidget* parent);
};
