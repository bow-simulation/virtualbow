#include "ColorPropertyItem.hpp"
#include "GroupPropertyItem.hpp"
#include "gui/limbview/LayerColors.hpp"
#include <QColorDialog>

ColorPropertyItem::ColorPropertyItem(QString name, GroupPropertyItem* parent)
    : PropertyTreeItem(parent),
      name(name),
      value(Qt::red)
{
    this->setFlags(this->flags() | Qt::ItemIsEditable);
}

QColor ColorPropertyItem::getValue() const {
    return value;
}

void ColorPropertyItem::setValue(const QColor& value) {
    this->value = value;
}

QVariant ColorPropertyItem::data(int column, int role) const {
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

void ColorPropertyItem::setData(int column, int role, const QVariant &value) {
    if(column == 1 && role == Qt::EditRole) {
        this->value = value.value<QColor>();
        emitDataChanged();
    }
}

QWidget* ColorPropertyItem::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    auto dialog = new QColorDialog(parent);
    dialog->setWindowModality(Qt::ApplicationModal);
    dialog->setWindowTitle("Color");

    return dialog;
}

void ColorPropertyItem::setEditorData(QWidget* editor, const QModelIndex& index) const {
    QColor value = index.model()->data(index, Qt::EditRole).value<QColor>();
    QColorDialog* dialog = static_cast<QColorDialog*>(editor);
    dialog->setCurrentColor(value);
}

void ColorPropertyItem::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    QColorDialog* dialog = static_cast<QColorDialog*>(editor);
    if(dialog->result() == QDialog::Accepted) {
        QColor value = dialog->currentColor();
        model->setData(index, value, Qt::EditRole);
    }
}
