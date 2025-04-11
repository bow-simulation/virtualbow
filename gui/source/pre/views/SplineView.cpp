#include "SplineView.hpp"
#include "pre/widgets/TableDelegate.hpp"
#include "primitive/TableView.hpp"
#include "pre/widgets/TableModel.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"

SplineView::SplineView(TableModel* model) {
    auto tableView = new TableView2();
    tableView->setModel(model);
    tableView->setItemDelegateForColumn(0, new TableDelegate(Quantities::length, DoubleRange::unrestricted(0.1e-3)));
    tableView->setItemDelegateForColumn(1, new TableDelegate(Quantities::length, DoubleRange::unrestricted(0.1e-3)));

    addWidget(
        "TODO: Some tooltip",
        tableView
    );
}
