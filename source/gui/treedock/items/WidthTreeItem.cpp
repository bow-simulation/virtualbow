#include "WidthTreeItem.hpp"
#include "gui/viewmodel/ViewModel.hpp"
#include "gui/widgets/TableEditor.hpp"
#include "gui/plotdock/plots/SplineView.hpp"
#include "gui/viewmodel/units/UnitSystem.hpp"

WidthTreeItem::WidthTreeItem(ViewModel* model)
    : TreeItem(model, "Width", QIcon(":/icons/model-width.svg"), TreeItemType::WIDTH)
{
    auto table = new TableEditor("Length", "Width", Quantities::ratio, Quantities::length,  DoubleRange::nonNegative(1e-4), DoubleRange::positive(1e-4));
    auto plot  = new SplineView("Length", "Width", Quantities::ratio, Quantities::length);

    setEditor(table);
    setPlot(plot);

    updateView(nullptr);

    QObject::connect(table, &TableEditor::modified, [=] {
        updateModel(this);
    });
    QObject::connect(model, &ViewModel::widthModified, [=](void* source){
        updateView(source);
    });
}

void WidthTreeItem::updateModel(void* source) {
    auto table_editor = static_cast<TableEditor*>(editor);
    model->setWidth(table_editor->getData(), source);
}

void WidthTreeItem::updateView(void* source) {
    if(source != this) {
        // Update editor
        auto table_editor = static_cast<TableEditor*>(editor);
        table_editor->setData(model->getWidth());
    }

    // Update plot
    auto spline_view = static_cast<SplineView*>(plot);
    spline_view->setData(model->getWidth());
}
