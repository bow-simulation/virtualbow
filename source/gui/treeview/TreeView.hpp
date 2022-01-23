#pragma once
#include <QTreeWidget>
#include <QHeaderView>
#include <functional>

class DataViewModel;

class TreeView: public QTreeWidget {
    Q_OBJECT

public:
    TreeView(DataViewModel* model);

private:
    DataViewModel* model;

    void createTopLevelItems();
    void createMaterialItems(QTreeWidgetItem* parent);
    void createLayerItems(QTreeWidgetItem* parent);
    void createProfileItems(QTreeWidgetItem* parent);


};
