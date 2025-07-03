#include "DoubleView.hpp"
#include <QAbstractItemModel>
#include <cmath>

//calculate::Parser DoubleView::parser = calculate::Parser{};

DoubleView::DoubleView(QAbstractItemModel* model, QPersistentModelIndex index, const Quantity& quantity, const DoubleRange& range):
    DoubleSpinBox(quantity, range)
{
    // Keep value up to date
    QObject::connect(this, &QDoubleSpinBox::valueChanged, this, [=](double value){ model->setData(index, value); });
    setValue(model->data(index, Qt::DisplayRole).toDouble());
}
