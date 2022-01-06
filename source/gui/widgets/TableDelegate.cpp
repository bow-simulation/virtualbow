#include "TableDelegate.hpp"
#include <QDoubleSpinBox>

TableDelegate::TableDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

QWidget* TableDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
    editor->setFrame(false);
    //editor->setMinimum(0);
    //editor->setMaximum(100);

    QObject::connect(editor, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double value){
        const_cast<TableDelegate*>(this)->commitData(editor);    // Hack
    });

    return editor;
}

void TableDelegate::setEditorData(QWidget* editor, const QModelIndex &index) const {
    int value = index.model()->data(index, Qt::EditRole).toDouble();

    QDoubleSpinBox *spinbox = static_cast<QDoubleSpinBox*>(editor);
    spinbox->setValue(value);
}

void TableDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    QDoubleSpinBox *spinbox = static_cast<QDoubleSpinBox*>(editor);
    spinbox->interpretText();
    double value = spinbox->value();

    model->setData(index, value, Qt::EditRole);
}

void TableDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    editor->setGeometry(option.rect);
}
