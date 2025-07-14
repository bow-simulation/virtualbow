#include "SplineView.hpp"
#include "primitive/TableView.hpp"
#include "pre/models/TableModel.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/models/units/UnitSystem.hpp"

SplineView::SplineView(TableModel* model) {
    auto tableView = new TableView();
    tableView->setToolTip("Control points of the spline curve");
    tableView->setModel(model);
    tableView->setItemDelegateForColumn(0, new TableDelegate(Quantities::length, DoubleRange::unrestricted(1e-3)));
    tableView->setItemDelegateForColumn(1, new TableDelegate(Quantities::length, DoubleRange::unrestricted(1e-3)));

    addWidget(tableView);
}
