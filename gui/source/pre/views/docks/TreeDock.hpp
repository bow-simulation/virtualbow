#pragma once
#include <QDockWidget>
#include <QTreeWidgetItem>

class MainTreeModel;
class QTreeView;
class QToolButton;
class QAction;
class QMenu;

class TreeDock: public QDockWidget {
public:
    TreeDock(MainTreeModel* viewModel, QItemSelectionModel* selectionModel);

private:
    MainTreeModel* viewModel;
    QTreeView* tree;

    QToolButton* buttonAdd;
    QAction* actionRemove;
    QAction* actionMoveUp;
    QAction* actionMoveDown;

    QMenu* menuAddMaterial;
    QMenu* menuAddLayer;
    QMenu* menuAddSegment;

    QMenu* createMaterialMenu();
    QMenu* createLayerMenu();
    QMenu* createSegmentMenu();

    void updateActions();
};
