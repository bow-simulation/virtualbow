#pragma once
#include <QDockWidget>
#include <QTreeWidgetItem>

class DataViewModel;
class QTreeWidget;
class QTreeWidgetItem;
class QToolButton;
class QMenu;
class TreeItem;

// TODO: Forward-declare
#include "gui/viewmodel/DataViewModel.hpp"

enum Types {
    Comments = QTreeWidgetItem::UserType,  // Required by Qt
    Settings,
    Dimensions,
    Materials,
    Material,
    Layers,
    Layer,
    Profile,
    Segment,
    Width,
    String,
    Masses,
    Damping
};

// TODO:
// * Items: Set Icon from type
// * Items: Add createEditor and createPlot that creates editor by type
// * Tree: Implement segment operations in viewmodel

// Bugs:
// * Remove crashes when there are no child items

#include <QLabel>

class TreeItem: public QTreeWidgetItem
{
public:
    TreeItem(const QString& name, const QIcon& icon, Types type)
        : QTreeWidgetItem({name}, type)
    {
        this->setIcon(0, icon);
    }

    QWidget* getEditor() {
        if(editor == nullptr) {
            editor = new QLabel(this->text(0));
        }
        return editor;
    }

    QWidget* getPlot() {
        if(plot == nullptr) {
            plot = new QLabel(this->text(0));
        }
        return plot;
    }


private:
    QWidget* editor = nullptr;
    QWidget* plot = nullptr;
};

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
    QTreeWidgetItem* createMaterialItem(const struct Material& material) const;
    QTreeWidgetItem* createLayerItem(const struct Layer& layer) const;
    QTreeWidgetItem* createProfileItem(const SegmentInput& segment) const;

    void swapTreeItems(QTreeWidgetItem* parent, int i, int j);
    void updateButtons(TreeItem* selection);

    void addMaterial(int index);
    void removeMaterial(int index);
    void swapMaterials(int i, int j);

    void addLayer(int index);
    void removeLayer(int index);
    void swapLayers(int i, int j);

    void addSegment(int index, const SegmentInput& segment);
    void removeSegment(int index);
    void swapSegments(int i, int j);
};
