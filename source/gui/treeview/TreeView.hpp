#pragma once
#include "TreeItem.hpp"
#include "solver/model/input/InputData.hpp"    // TODO: Forward declare?
#include <QDockWidget>
#include <QTreeWidgetItem>

class DataViewModel;
class QTreeWidget;
class QTreeWidgetItem;
class QToolButton;
class QMenu;
class TreeItem;

// TODO:
// * Items: Set Icon from type
// * Items: Add createEditor and createPlot that creates editor by type
// * Tree: Implement segment operations in viewmodel

class TreeView: public QDockWidget
{
    Q_OBJECT

public:
    TreeView(DataViewModel* model);

signals:
    void currentEditorChanged(QWidget* editor);
    void currentPlotChanged(QWidget* plot);

private:
    DataViewModel* model;
    QTreeWidget* tree;

    QToolButton* button_add;
    QToolButton* button_remove;
    QToolButton* button_up;
    QToolButton* button_down;
    QMenu* segment_menu;

    QTreeWidgetItem* item_comments;
    QTreeWidgetItem* item_settings;
    QTreeWidgetItem* item_dimensions;
    QTreeWidgetItem* item_materials;
    QTreeWidgetItem* item_layers;
    QTreeWidgetItem* item_profile;
    QTreeWidgetItem* item_width;
    QTreeWidgetItem* item_string;
    QTreeWidgetItem* item_masses;
    QTreeWidgetItem* item_damping;

    void createSegmentMenu();
    void createTopLevelItems();
    QTreeWidgetItem* createMaterialItem(const Material& material) const;
    QTreeWidgetItem* createLayerItem(const Layer& layer) const;
    QTreeWidgetItem* createProfileItem(const SegmentInput& segment) const;

    QString createUniqueName(const QString& name, QTreeWidgetItem* parent) const;

    void swapTreeItems(QTreeWidgetItem* parent, int i, int j);
    void insertTreeItem(QTreeWidgetItem* parent, QTreeWidgetItem* item, int index);
    void removeTreeItem(QTreeWidgetItem* parent, int index);

    void updateButtons();

    void insertMaterial(int index);
    void removeMaterial(int index);
    void swapMaterials(int i, int j);

    void insertLayer(int index);
    void removeLayer(int index);
    void swapLayers(int i, int j);

    void insertSegment(int index, const SegmentInput& segment);
    void removeSegment(int i);
    void swapSegments(int i, int j);
};
