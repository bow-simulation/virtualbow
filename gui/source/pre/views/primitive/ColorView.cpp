#include "ColorView.hpp"
#include <QAbstractItemModel>
#include <QColorDialog>
#include <QPalette>

ColorView::ColorView(QAbstractItemModel* model, QPersistentModelIndex index, const QString& tooltip) {
    setToolTip(tooltip);

    // Set initial color value from model
    QColor initial = model->data(index).value<QColor>();
    setColor(initial);

    // Open solor dialog on click
    QObject::connect(this, &QPushButton::clicked, this, [=, this]{
        auto dialog(new QColorDialog(this));
        dialog->setOption(QColorDialog::DontUseNativeDialog);
        dialog->setWindowModality(Qt::ApplicationModal);
        dialog->setWindowTitle("Color");
        dialog->setCurrentColor(model->data(index).value<QColor>());

        // Update model and button on color changes
        QObject::connect(dialog, &QColorDialog::currentColorChanged, this, [=, this] {
            model->setData(index, dialog->currentColor());
            setColor(initial);
        });

        // When the dialog is closed, either keep the selected color or reset to initial color
        if(dialog->exec() == QDialog::Accepted) {
            setColor(dialog->currentColor());
        } else {
            model->setData(index, initial);
        }
    });
}

void ColorView::setColor(const QColor& color) {
    // Set button text to show color code
    setText(color.name());

    // Set background color of the button to the given color
    QPalette pal = palette();
    pal.setColor(QPalette::Button, color);

    // Set the text color depending on brightness of the background
    if(color.lightness() < 200) {
        pal.setColor(QPalette::ButtonText, Qt::white);
    }
    else {
        pal.setColor(QPalette::ButtonText, Qt::darkGray);
    }

    setPalette(pal);
}
