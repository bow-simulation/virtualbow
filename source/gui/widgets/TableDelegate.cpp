#include "TableDelegate.hpp"
#include "gui/viewmodel/units/UnitGroup.hpp"
#include "gui/widgets/DoubleSpinBox.hpp"

TableDelegate::TableDelegate(const UnitGroup& units, const DoubleRange& range, QObject* parent)
    : QStyledItemDelegate(parent),
      units(units),
      range(range)
{

}

QWidget* TableDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    auto editor = new DoubleSpinBox(units, range, parent);
    editor->setFrame(false);

    QObject::connect(editor, &DoubleSpinBox::stepped, this, [=]{
        const_cast<TableDelegate*>(this)->commitData(editor);    // Hack
    });

    return editor;
}

void TableDelegate::setEditorData(QWidget* editor, const QModelIndex &index) const {
    double value = index.model()->data(index, Qt::EditRole).toDouble();

    auto spinner = static_cast<DoubleSpinBox*>(editor);
    spinner->setValue(value);
}

void TableDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    auto spinner = static_cast<DoubleSpinBox*>(editor);
    spinner->interpretText();    // From Qt tutorial
    double value = spinner->value();

    model->setData(index, value, Qt::EditRole);
}

void TableDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    editor->setGeometry(option.rect);
}
