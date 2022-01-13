#pragma once
#include <QTreeWidget>
#include <QHeaderView>

class ViewModel;

class TreeView: public QTreeWidget {
    Q_OBJECT

public:
    TreeView(ViewModel* model);

    void createTopLevelItems();
    void createMaterialItems(QTreeWidgetItem* parent);
    void createProfileItems(QTreeWidgetItem* parent);
    void createLayerItems(QTreeWidgetItem* parent);
};
