#pragma once
#include "solver/model/input/InputData.hpp"
#include "gui/units/UnitSystem.hpp"
#include "gui/dialogs/CommentDialog.hpp"
#include "gui/dialogs/ProfileDialog.hpp"
#include "gui/dialogs/WidthDialog.hpp"
#include "gui/dialogs/LayerDialog.hpp"
#include "gui/dialogs/SettingsDialog.hpp"
#include "gui/dialogs/StringDialog.hpp"
#include "gui/dialogs/MassesDialog.hpp"
#include "gui/dialogs/DampingDialog.hpp"
#include "gui/dialogs/DimensionsDialog.hpp"
#include <QtWidgets>

class DialogTreeItem: public QTreeWidgetItem
{
public:
    DialogTreeItem(QTreeWidget* parent, const QString& name, const QIcon& icon, QDialog* dialog);
    void showDialog();

private:
    QDialog* dialog;
};

class TreeEditor: public QTreeWidget {
    Q_OBJECT

public:
    TreeEditor();
    InputData getData() const;
    void setData(const InputData& data);
    void setUnits(const UnitSystem& units);

signals:
    void modified();

private:
    CommentDialog* dialog_comments = new CommentDialog(this);
    SettingsDialog* dialog_settings = new SettingsDialog(this);
    DimensionsDialog* dialog_dimensions = new DimensionsDialog(this);
    ProfileDialog* dialog_profile = new ProfileDialog(this);
    WidthDialog* dialog_width = new WidthDialog(this);
    LayerDialog* dialog_layers = new LayerDialog(this);
    StringDialog* dialog_string = new StringDialog(this);
    MassesDialog* dialog_masses = new MassesDialog(this);
    DampingDialog* dialog_damping = new DampingDialog(this);    
};
