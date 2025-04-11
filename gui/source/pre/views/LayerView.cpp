#include "LayerView.hpp"
#include "pre/viewmodels/LayerVM.hpp"
#include "pre/widgets/TableDelegate.hpp"
#include "primitive/StringSelectionView.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"
#include "primitive/TableView.hpp"
#include "pre/widgets/TableModel.hpp"
#include <QLabel>
#include <QTableView>

LayerView::LayerView(LayerVM* model, TableModel* tableModel) {
    addProperty(
        "Material",
        "Material that is assigned to this layer",
        new StringSelectionView(model, model->MATERIAL, model->materialOptions())
    );

    auto tableView = new TableView2();
    tableView->setModel(tableModel);
    tableView->setItemDelegateForColumn(0, new TableDelegate(Quantities::ratio, DoubleRange::inclusive(0.0, 1.0, 0.01)));
    tableView->setItemDelegateForColumn(1, new TableDelegate(Quantities::length, DoubleRange::nonNegative(0.1e-3)));

    addWidget(
        "TODO: Some tooltip",
        tableView
    );
}
