#include "DoubleView.hpp"
#include <QAbstractItemModel>
#include <cmath>

//calculate::Parser DoubleView::parser = calculate::Parser{};

DoubleView::DoubleView(QAbstractItemModel* model, QPersistentModelIndex index, const Quantity& quantity, const DoubleRange& range):
    DoubleSpinBox(quantity, range)
{
    // Set value from model and keep model up to date on changes
    setValue(model->data(index, Qt::DisplayRole).toDouble());
    QObject::connect(this, &QDoubleSpinBox::valueChanged, this, [=](double value){ model->setData(index, value); });
}
