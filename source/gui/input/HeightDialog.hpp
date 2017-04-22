#pragma once
#include "gui/input/SeriesView.hpp"
#include "gui/input/SplineView.hpp"
#include "model/InputData.hpp"

#include <QtWidgets>

class InputData;

class HeightDialog: public QDialog
{
public:
    HeightDialog(QWidget* parent, InputData& data)
        : QDialog(parent)
    {
        // Widgets

        auto series_view = new SeriesView("Rel. position", "Height [m]", data.sections_height);
        auto spline_view = new SplineView("Relative position", "Height [m]", data.sections_height);
        QObject::connect(series_view, &SeriesView::selectionChanged, spline_view, &SplineView::setMarkedControlPoints);

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
