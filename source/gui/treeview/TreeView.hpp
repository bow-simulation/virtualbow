#pragma once
#include <QTreeView>

class ViewModel;

class TreeView: public QTreeView {
    Q_OBJECT

public:
    TreeView(ViewModel* model);
};

