#include "WidthTreeItem.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include "gui/widgets/TableEditor.hpp"
#include "gui/widgets/SplineView.hpp"
#include "gui/units/UnitSystem.hpp"

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

    updateView();
    QObject::connect(table, &TableEditor::modified, [&] { updateModel(); });
}

void WidthTreeItem::updateModel() {
    model->setWidth(static_cast<TableEditor*>(editor)->getData());
}

void WidthTreeItem::updateView() {
    static_cast<TableEditor*>(editor)->setData(model->getWidth());
}
