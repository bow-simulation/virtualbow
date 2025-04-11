#include "WidthView.hpp"
#include "pre/widgets/TableDelegate.hpp"
#include "primitive/TableView.hpp"
#include "pre/widgets/TableModel.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"

WidthView::WidthView(TableModel* model) {
    auto tableView = new TableView2();
    tableView->setModel(model);
    tableView->setItemDelegateForColumn(0, new TableDelegate(Quantities::ratio, DoubleRange::inclusive(0.0, 1.0, 0.01)));
    tableView->setItemDelegateForColumn(1, new TableDelegate(Quantities::length, DoubleRange::nonNegative(0.1e-3)));

    addWidget(
        "TODO: Some tooltip",
        tableView
    );
}
