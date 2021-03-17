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
    TreeEditor(const UnitSystem& units);
    InputData getData() const;
    void setData(const InputData& data);

signals:
    void modified();

private:
    CommentDialog* dialog_comments;
    SettingsDialog* dialog_settings;
    DimensionsDialog* dialog_dimensions;
    ProfileDialog* dialog_profile;
    WidthDialog* dialog_width;
    LayerDialog* dialog_layers;
    StringDialog* dialog_string;
    MassesDialog* dialog_masses;
    DampingDialog* dialog_damping;
};
