#include "ColorTreeItem.hpp"
#include "GroupTreeItem.hpp"
#include "gui/limbview/LayerColors.hpp"
#include <QColorDialog>

ColorTreeItem::ColorTreeItem(QString name, GroupTreeItem* parent)
    : PropertyTreeItem(parent),
      name(name),
      value(Qt::red)
{
    this->setFlags(this->flags() | Qt::ItemIsEditable);
}

QVariant ColorTreeItem::data(int column, int role) const {
    if(column == 0 && role == Qt::DisplayRole) {
        return name;
    }

    if(column == 1) {
        if(role == Qt::EditRole) {
            return value;
        }
        if(role == Qt::DecorationRole) {
            return getColorPixmap(value, 16);
        }
    }

    return PropertyTreeItem::data(column, role);
}

void ColorTreeItem::setData(int column, int role, const QVariant &value) {
    if(column == 1 && role == Qt::EditRole) {
        this->value = value.value<QColor>();
    }
}

QWidget* ColorTreeItem::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    auto dialog = new QColorDialog(parent);
    dialog->setWindowModality(Qt::ApplicationModal);
    dialog->setWindowTitle("Color");

    return dialog;
}

void ColorTreeItem::setEditorData(QWidget* editor, const QModelIndex& index) const {
    QColor value = index.model()->data(index, Qt::EditRole).value<QColor>();
    QColorDialog* dialog = static_cast<QColorDialog*>(editor);
    dialog->setCurrentColor(value);
}

void ColorTreeItem::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    QColorDialog* dialog = static_cast<QColorDialog*>(editor);
    if(dialog->result() == QDialog::Accepted) {
        QColor value = dialog->currentColor();
        model->setData(index, value, Qt::EditRole);
    }
}
