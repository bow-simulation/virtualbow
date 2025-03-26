#pragma once
#include "TreeItem.hpp"
#include <QDockWidget>
#include <QTreeWidgetItem>

class ModelTreeVM;
class ViewModel;
class QTreeView;
class QTreeWidgetItem;
class QToolButton;
class QMenu;

class TreeItem;
class CommentTreeItem;
class SettingsTreeItem;
class DimensionsTreeItem;
class MaterialsTreeItem;
class StringTreeItem;
class MassesTreeItem;
class DampingTreeItem;
class WidthTreeItem;
class LayersTreeItem;
class ProfileTreeItem;

class TreeDock: public QDockWidget
{
    Q_OBJECT

public:
    TreeDock(ModelTreeVM* viewModel);

signals:
    void currentEditorChanged(QWidget* editor);
    void currentPlotChanged(QWidget* plot);

private:
    ViewModel* model;
    QTreeView* tree;

    QToolButton* button_add;
    QToolButton* button_remove;
    QToolButton* button_up;
    QToolButton* button_down;

    QMenu* menu_add_material;
    QMenu* menu_add_layer;
    QMenu* menu_add_segment;

    CommentTreeItem* item_comments;
    SettingsTreeItem* item_settings;
    DimensionsTreeItem* item_dimensions;
    MaterialsTreeItem* item_materials;
    LayersTreeItem* item_layers;
    ProfileTreeItem* item_profile;
    WidthTreeItem* item_width;
    StringTreeItem* item_string;
    MassesTreeItem* item_masses;
    DampingTreeItem* item_damping;

    QMenu* createMaterialMenu();
    QMenu* createLayerMenu();
    QMenu* createSegmentMenu();

    void createTopLevelItems();
    void updateButtons();
};
