#pragma once
#include <QComboBox>

class StringSelectionView: public QComboBox {
    Q_OBJECT

public:
    StringSelectionView(QAbstractItemModel* model, QPersistentModelIndex index, const QStringList& texts);
};
