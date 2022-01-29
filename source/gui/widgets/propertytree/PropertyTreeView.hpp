#pragma once
#include <QTreeView>

class PropertyTreeModel;

class PropertyTreeView: public QTreeView {
public:
    PropertyTreeView(PropertyTreeModel* model);
};

