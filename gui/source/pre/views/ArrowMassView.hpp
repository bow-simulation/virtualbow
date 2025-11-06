#pragma once
#include <QWidget>

class QAbstractItemModel;

class ArrowMassView: public QWidget {
public:
    ArrowMassView(QAbstractItemModel* model, QPersistentModelIndex index);
};
