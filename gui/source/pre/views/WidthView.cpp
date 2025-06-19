#include "WidthView.hpp"
#include "primitive/TableView.hpp"
#include "pre/models/TableModel.hpp"
#include "pre/utils/DoubleRange.hpp"
#include "pre/models/units/UnitSystem.hpp"

WidthView::WidthView(TableModel* model) {
    auto tableView = new TableView();
    tableView->setModel(model);
    tableView->setItemDelegateForColumn(0, new TableDelegate(Quantities::ratio, DoubleRange::inclusive(0.0, 1.0, 0.1e-3)));
    tableView->setItemDelegateForColumn(1, new TableDelegate(Quantities::length, DoubleRange::positive(0.1e-3)));

    addWidget(
        "Limb width over relative position along the limb",
        tableView
    );
}
