#include "ColorView.hpp"
#include <QAbstractItemModel>
#include <QColorDialog>
#include <QPalette>

ColorView::ColorView(QAbstractItemModel* model, QPersistentModelIndex index) {
    setColor(model->data(index).value<QColor>());
    QObject::connect(this, &QPushButton::clicked, this, [=]{
        auto dialog(new QColorDialog(this));
        dialog->setOption(QColorDialog::DontUseNativeDialog);
        dialog->setWindowModality(Qt::ApplicationModal);
        dialog->setWindowTitle("Color");
        dialog->setCurrentColor(model->data(index).value<QColor>());
        dialog->exec();

        model->setData(index, dialog->currentColor());
        setColor(dialog->currentColor());
    });
}

void ColorView::setColor(const QColor& color) {
    // Set background color of the button to the given color and the text color depending on brightness
    QPalette pal = palette();
    pal.setColor(QPalette::Button, color);
    if(color.lightness() < 200) {
        pal.setColor(QPalette::ButtonText, Qt::white);
    }
    else {
        pal.setColor(QPalette::ButtonText, Qt::darkGray);
    }

    setPalette(pal);

    // Set button text to show color code
    setText(color.name());
}
