#include "DoubleView.hpp"
#include <QAbstractItemModel>
#include <cmath>

DoubleView::DoubleView(QAbstractItemModel* model, QPersistentModelIndex index, const Quantity& quantity, const DoubleRange& range, const QString& tooltip):
    DoubleSpinBox(quantity, range)
{
    setToolTip(tooltip);

    // Set value from model and keep model up to date on changes
    setValue(model->data(index, Qt::DisplayRole).toDouble());
    QObject::connect(this, &QDoubleSpinBox::valueChanged, this, [=](double value){ model->setData(index, value); });
}
