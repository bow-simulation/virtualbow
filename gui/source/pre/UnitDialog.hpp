#pragma once
#include <QWidget>
#include <QDialog>

class UnitDialog;
class Quantity;

class UnitEditor: public QWidget {
public:
    UnitEditor(Quantity& group);
};

class UnitDialog: public QDialog {
    Q_OBJECT
public:
    UnitDialog(QWidget* parent);
};
