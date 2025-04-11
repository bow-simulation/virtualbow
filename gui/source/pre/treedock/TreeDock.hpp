#pragma once
#include <QDockWidget>
#include <QTreeWidgetItem>

class ModelTreeVM;
class QTreeView;
class QToolButton;
class QAction;
class QMenu;

class TreeDock: public QDockWidget {
public:
    TreeDock(ModelTreeVM* viewModel, QItemSelectionModel* selectionModel);

private:
    ModelTreeVM* viewModel;
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
