#pragma once
#include <QPlainTextEdit>

class QAbstractItemModel;

class TextView: public QPlainTextEdit
{
public:
    TextView(QAbstractItemModel* model, QPersistentModelIndex index);
};
