#include "TableDelegate.hpp"
#include "gui/UnitDialog.hpp"

TableDelegate::TableDelegate(const TableSpinnerOptions& options, QObject* parent)
    : QStyledItemDelegate(parent),
      options(options)
{

}

QWidget* TableDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    TableSpinner* editor = new TableSpinner(options, parent);
    editor->setFrame(false);

    QObject::connect(editor, QOverload<double>::of(&TableSpinner::valueChanged), this, [=](double value){
        const_cast<TableDelegate*>(this)->commitData(editor);    // Hack
    });

    return editor;
}

void TableDelegate::setEditorData(QWidget* editor, const QModelIndex &index) const {
    double value = index.model()->data(index, Qt::EditRole).toDouble();

    TableSpinner* spinner = static_cast<TableSpinner*>(editor);
    spinner->setValue(value);
}

void TableDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    TableSpinner* spinner = static_cast<TableSpinner*>(editor);
    spinner->interpretText();    // From Qt tutorial
    double value = spinner->value();

    model->setData(index, value, Qt::EditRole);
}

void TableDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    editor->setGeometry(option.rect);
}
