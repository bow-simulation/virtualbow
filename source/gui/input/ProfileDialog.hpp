#pragma once
#include "gui/input/SeriesView.hpp"
#include "gui/input/ProfileView.hpp"
#include "gui/input/DoubleView.hpp"
#include "model/InputData.hpp"

#include <QtWidgets>

class InputData;

class ProfileDialog: public QDialog
{
public:
    ProfileDialog(QWidget* parent, InputData& data)
        : QDialog(parent)
    {
        // Widgets

        auto series_view = new SeriesView("Length [m]", "Curvature [m⁻¹]", data.profile_segments);

        auto plot = new ProfileView(data);

        auto hbox0 = new QHBoxLayout();
        hbox0->addStretch();
        hbox0->addWidget(new QLabel("X0 [m]:")); hbox0->addSpacing(10); hbox0->addWidget(new DoubleView(data.profile_x0));
        hbox0->addSpacing(10);
        hbox0->addStretch();
        hbox0->addWidget(new QLabel("Y0 [m]:")); hbox0->addSpacing(10); hbox0->addWidget(new DoubleView(data.profile_y0));
        hbox0->addSpacing(10);
        hbox0->addStretch();
        hbox0->addWidget(new QLabel("Angle [rad]:")); hbox0->addSpacing(10); hbox0->addWidget(new DoubleView(data.profile_phi0));
        hbox0->addStretch();

        auto offsets = new QGroupBox();
        offsets->setLayout(hbox0);

        // QGroupBox{ margin-top: 0; }

        auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
        QObject::connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);

        // Layout

        auto vbox0 = new QVBoxLayout();
        //vbox0->setMargin(0);
        vbox0->addWidget(plot, 1);
        vbox0->addWidget(offsets);

        auto hbox1 = new QHBoxLayout();
        hbox1->addWidget(series_view);
        hbox1->addLayout(vbox0, 1);

        auto vbox1 = new QVBoxLayout();
        vbox1->addLayout(hbox1, 1);
        vbox1->addWidget(buttons);

        this->setWindowTitle("Profile");
        this->setLayout(vbox1);
    }
};
