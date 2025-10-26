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
        auto dialog(new QColorDialog());
        dialog->setOption(QColorDialog::DontUseNativeDialog);
        dialog->setWindowModality(Qt::ApplicationModal);
        dialog->setWindowTitle("Color");
        dialog->setCurrentColor(model->data(index).value<QColor>());

        // Update model and button on color changes
        QObject::connect(dialog, &QColorDialog::currentColorChanged, this, [=, this] {
            model->setData(index, dialog->currentColor());
            setColor(dialog->currentColor());
        });

        // When the dialog is closed, either keep the selected color or reset to initial color
        if(dialog->exec() == QDialog::Accepted) {
            setColor(dialog->currentColor());
        }
        else {
            model->setData(index, initial);
            setColor(initial);
        }
    });
}

void ColorView::setColor(const QColor& color) {
    // Set button text to show color in rgb format
    setText(QString("RGB %1, %2, %3").arg(color.red()).arg(color.green()).arg(color.blue()));

    // Pick text color depending on brightness of the background color
    // https://stackoverflow.com/a/1855903
    double luminance = 0.299*color.redF() + 0.587*color.greenF() + 0.114*color.blueF();
    QColor textColor = (luminance > 0.5) ? Qt::black : Qt::white;

    // Generate style string (using QPalette didn't change the background color on Windows)
    QString style = QString("background-color: %1; color: %2;")
        .arg(color.name())
        .arg(textColor.name());

    setStyleSheet(style);
}
