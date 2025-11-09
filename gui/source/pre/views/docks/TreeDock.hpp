#pragma once
#include <QDockWidget>
#include <QTreeWidgetItem>

class MainModel;
class MainTreeModel;
class QTreeView;
class QToolButton;
class QAction;
class QMenu;

class TreeDock: public QDockWidget {
public:
    TreeDock(MainModel* mainModel);

private:
    MainTreeModel* model;
    QTreeView* tree;

    QToolButton* buttonAdd;
    QAction* actionRemove;
    QAction* actionMoveUp;
    QAction* actionMoveDown;
    QAction* actionRename;

    QMenu* menuAddMaterial;
    QMenu* menuAddLayer;
    QMenu* menuAddSegment;

    QMenu* createMaterialMenu();
    QMenu* createLayerMenu();
    QMenu* createSegmentMenu();

    void updateActions();
};
