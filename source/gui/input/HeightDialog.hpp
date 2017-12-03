#pragma once
#include "gui/PersistentDialog.hpp"
#include "gui/views/SeriesView.hpp"
#include "gui/views/SplineView.hpp"
#include "bow/input/InputData.hpp"

#include <QtWidgets>

class InputData;

class HeightDialog: public PersistentDialog
{
public:
    HeightDialog(QWidget* parent, InputData& data)
        : PersistentDialog(parent, "HeightDialog", {800, 300})    // Magic numbers
    {
        // Widgets

        auto series_view = new SeriesView("Rel. position", "Height [m]", data.height);
        auto spline_view = new SplineView("Relative position", "Height [m]", data.height);
        QObject::connect(series_view, &SeriesView::selectionChanged, spline_view, &SplineView::setSelection);

        auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
        QObject::connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);

        // Layout

        auto hbox = new QHBoxLayout();
        hbox->addWidget(series_view);
        hbox->addWidget(spline_view, 1);

        auto vbox = new QVBoxLayout();
        vbox->addLayout(hbox, 1);
        vbox->addWidget(buttons);

        this->setWindowTitle("Height");
        this->setLayout(vbox);
    }
};
