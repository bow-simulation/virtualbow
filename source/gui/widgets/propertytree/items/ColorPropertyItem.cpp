#include "ColorPropertyItem.hpp"
#include "GroupPropertyItem.hpp"
#include "gui/limbview/LayerColors.hpp"
#include <QColorDialog>
#include <QApplication>

// According to the answers in [1], dialogs can be used directly as editors in a delegate.
// This is also supported by the internal Qt code shown in [2].
// Hovever, this approach turned out to have some issues:
//
// * On Linux it mostly worked, except on Linux Mint where the dialog would just disappear when clicking on it (probably an older Qt version)
// * On Windows, the dialog would pop up in a weird location with the title bar not visible, so couldn't be moved either. Didn't manage to move it in code.
// * On macOS it seemed to work without any issues
//
// So the other solution was picked: Create a wrapper widget that opens the dialog as a child.
// The final hurdle is to get the widget to trigger the item delegate when the dialog is finished, see [2].
//
// [1] https://stackoverflow.com/q/40264262
// [2] https://stackoverflow.com/q/30063133

class ColorEditor: public QWidget {
public:
    ColorEditor(QWidget* parent)
        : QWidget(parent),
          dialog(new QColorDialog(this))
    {
        // When the dialog is closed, send a QEvent::FocusOut event to trigger the item delegate
        // to update the model (using clearFocus() does not work).
        QObject::connect(dialog, &QColorDialog::finished, [=](int result) {
            QEvent event(QEvent::FocusOut);
            QApplication::sendEvent(this, &event);
        });

        dialog->setWindowModality(Qt::ApplicationModal);
        dialog->setWindowTitle("Color");
        dialog->show();
    }

    void setCurrentColor(const QColor& color) {
        dialog->setCurrentColor(color);
    }

    QColor currentColor() const {
        return dialog->currentColor();
    }

    int result() const {
        return dialog->result();
    }

private:
    QColorDialog* dialog;
};

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
    auto color_editor = new ColorEditor(parent);
    return color_editor;
}

void ColorPropertyItem::setEditorData(QWidget* editor, const QModelIndex& index) const {
    QColor value = index.model()->data(index, Qt::EditRole).value<QColor>();
    auto color_editor = static_cast<ColorEditor*>(editor);
    color_editor->setCurrentColor(value);
}

void ColorPropertyItem::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    auto color_editor = static_cast<ColorEditor*>(editor);
    if(color_editor->result() == QDialog::Accepted) {
        model->setData(index, color_editor->currentColor(), Qt::EditRole);
    }
}
