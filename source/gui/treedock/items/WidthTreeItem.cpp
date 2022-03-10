#include "WidthTreeItem.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include "gui/widgets/TableEditor.hpp"
#include "gui/plotdock/plots/SplineView.hpp"
#include "gui/viewmodel/units/UnitSystem.hpp"

WidthTreeItem::WidthTreeItem(DataViewModel* model)
    : TreeItem("Width", QIcon(":/icons/model-width.svg"), TreeItemType::WIDTH),
      model(model)
{
    auto table = new TableEditor("Position", "Width",
        TableSpinnerOptions(UnitSystem::ratio, DoubleRange::nonNegative(), 1e-4),
        TableSpinnerOptions(UnitSystem::length, DoubleRange::positive(), 1e-4)
    );
    auto plot  = new SplineView("Position", "Width", UnitSystem::ratio, UnitSystem::length);

    setEditor(table);
    setPlot(plot);

    updateEditor();
    updatePlot();
    QObject::connect(table, &TableEditor::modified, [&] {
        updateModel();
        updatePlot();
    });


}

void WidthTreeItem::updateModel() {
    model->setWidth(static_cast<TableEditor*>(editor)->getData());
}

void WidthTreeItem::updateEditor() {
    static_cast<TableEditor*>(editor)->setData(model->getWidth());
}

void WidthTreeItem::updatePlot() {
    static_cast<SplineView*>(plot)->setData(model->getWidth());
}
