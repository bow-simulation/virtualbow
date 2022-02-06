#pragma once
#include <QTreeWidgetItem>

enum TreeItemType {
    COMMENTS = QTreeWidgetItem::UserType,  // Required by Qt
    SETTINGS,
    DIMENSIONS,
    MATERIALS,
    MATERIAL,
    LAYERS,
    LAYER,
    PROFILE,
    SEGMENT,
    WIDTH,
    STRING,
    MASSES,
    DAMPING
};

class TreeItem: public QTreeWidgetItem {
public:
    TreeItem(const QString& name, const QIcon& icon, TreeItemType type);

    QWidget* getEditor();
    QWidget* getPlot();

private:
    QWidget* editor = nullptr;
    QWidget* plot = nullptr;
};
