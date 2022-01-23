#pragma once
#include <QDockWidget>

class DataViewModel;
class QTreeWidget;
class QTreeWidgetItem;

class TreeView: public QDockWidget {
    Q_OBJECT

public:
    TreeView(DataViewModel* model);

private:
    DataViewModel* model;
    QTreeWidget* tree;

    void createTopLevelItems();
    void createMaterialItems(QTreeWidgetItem* parent);
    void createLayerItems(QTreeWidgetItem* parent);
    void createProfileItems(QTreeWidgetItem* parent);
};
