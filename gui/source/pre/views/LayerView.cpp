#include "LayerView.hpp"
#include "pre/models/LayerModel.hpp"
#include "primitive/StringSelectionView.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "primitive/TableView.hpp"
#include "pre/models/TableModel.hpp"
#include <QLabel>
#include <QTableView>

LayerView::LayerView(LayerModel* model, TableModel* tableModel) {
    addProperty(
        "Material",
        "Material that is assigned to this layer",
        new StringSelectionView(model, model->MATERIAL, model->materialOptions())
    );

    auto tableView = new TableView();
    tableView->setModel(tableModel);
    tableView->setItemDelegateForColumn(0, new TableDelegate(Quantities::ratio, DoubleRange::inclusive(0.0, 1.0, 1e-4)));
    tableView->setItemDelegateForColumn(1, new TableDelegate(Quantities::length, DoubleRange::nonNegative(1e-4)));

    addWidget(
        "Layer height over relative position along the limb",
        tableView
    );
}
