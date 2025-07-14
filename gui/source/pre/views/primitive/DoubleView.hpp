#pragma once
#include "pre/widgets/DoubleSpinBox.hpp"

class QAbstractItemModel;

class DoubleView: public DoubleSpinBox {
public:
    DoubleView(QAbstractItemModel* model, QPersistentModelIndex index, const Quantity& quantity, const DoubleRange& range, const QString& tooltip);
};
