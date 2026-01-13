#include "IntegerView.hpp"
#include "pre/utils/IntegerRange.hpp"
#include <QAbstractItemModel>
#include <QWheelEvent>
#include <cmath>

IntegerView::IntegerView(QAbstractItemModel* model, QPersistentModelIndex index, const IntegerRange& range, const QString& tooltip):
    IntegerSpinBox(range)
{
    setToolTip(tooltip);

    // Set value from model and keep model up to date on changes
    setValue(model->data(index, Qt::DisplayRole).toInt());
    QObject::connect(this, &IntegerSpinBox::contentModified, this, [=, this]{ model->setData(index, this->value()); });
}
