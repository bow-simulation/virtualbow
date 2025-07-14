#include "LayerView.hpp"
#include "pre/models/LayerModel.hpp"
#include "primitive/StringSelectionView.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "primitive/TableView.hpp"
#include "pre/models/TableModel.hpp"
#include <QLabel>
#include <QTableView>

LayerView::LayerView(LayerModel* model, TableModel* tableModel) {
    // Generate tooltip for each material
    QStringList tooltips;
    for(QString material: model->materialOptions()) {
        tooltips.push_back("Assign material \"" + material + "\" to this layer");
    }

    addProperty(
        "Material",
        new StringSelectionView(model, model->MATERIAL, "Material that is assigned to this layer", model->materialOptions(), tooltips)
    );

    auto tableView = new TableView();
    tableView->setToolTip("Layer height over relative position along the limb");
    tableView->setModel(tableModel);
    tableView->setItemDelegateForColumn(0, new TableDelegate(Quantities::ratio, DoubleRange::inclusive(0.0, 1.0, 1e-4)));
    tableView->setItemDelegateForColumn(1, new TableDelegate(Quantities::length, DoubleRange::nonNegative(1e-4)));

    addWidget(tableView);
}
