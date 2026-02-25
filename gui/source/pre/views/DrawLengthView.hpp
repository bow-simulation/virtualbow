#pragma once
#include <QWidget>

class QAbstractItemModel;

class DrawLengthView: public QWidget {
public:
    DrawLengthView(QAbstractItemModel* model, QPersistentModelIndex index);
};
