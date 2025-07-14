#pragma once
#include "pre/widgets/IntegerSpinBox.hpp"

class QAbstractItemModel;

class IntegerView: public IntegerSpinBox {
public:
    IntegerView(QAbstractItemModel* model, QPersistentModelIndex index, const IntegerRange& range, const QString& tooltip);
};
