#include "SplineView.hpp"
#include "primitive/TableView.hpp"
#include "pre/models/TableModel.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "pre/Language.hpp"

SplineView::SplineView(TableModel* model) {
    auto tableView = new TableView();
    tableView->setToolTip(Tooltips::ProfileSplineInput);
    tableView->setModel(model);
    tableView->setItemDelegateForColumn(0, new TableDelegate(Quantities::length, DoubleRange::unrestricted(1e-3)));
    tableView->setItemDelegateForColumn(1, new TableDelegate(Quantities::length, DoubleRange::unrestricted(1e-3)));

    addWidget(tableView);
}
