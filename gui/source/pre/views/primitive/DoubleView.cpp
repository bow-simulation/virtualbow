#include "DoubleView.hpp"
#include <QAbstractItemModel>
#include <cmath>

DoubleView::DoubleView(QAbstractItemModel* model, QPersistentModelIndex index, const Quantity& quantity, const DoubleRange& range, const QString& tooltip):
    DoubleSpinBox(quantity, range)
{
    setToolTip(tooltip);

    // Set value from model and keep model up to date on changes
    setValue(model->data(index, Qt::DisplayRole).toDouble());
    QObject::connect(this, &DoubleSpinBox::contentModified, this, [=, this]{ model->setData(index, this->value()); });
}
