#include "IntegerView.hpp"
#include "pre/utils/IntegerRange.hpp"
#include <QAbstractItemModel>
#include <QWheelEvent>
#include <cmath>

//calculate::Parser IntegerView::parser = calculate::Parser{};

IntegerView::IntegerView(QAbstractItemModel* model, QPersistentModelIndex index, const IntegerRange& range):
    IntegerSpinBox(range)
{
    // Keep value up to date
    QObject::connect(this, &QSpinBox::valueChanged, this, [=](int value){ model->setData(index, value); });
    setValue(model->data(index, Qt::DisplayRole).toInt());
}
