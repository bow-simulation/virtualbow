#pragma once
#include "gui/PersistentDialog.hpp"
#include "gui/views/SeriesView.hpp"
#include "gui/views/ProfileView.hpp"
#include "gui/views/DoubleView.hpp"
#include "bow/input/InputData.hpp"

#include <QtWidgets>

class InputData;

class ProfileDialog: public PersistentDialog
{
public:
    ProfileDialog(QWidget* parent, InputData& data)
        : PersistentDialog(parent, "ProfileDialog", {800, 400})    // Magic numbers
    {
        // Widgets

        auto series_view = new SeriesView("Length [m]", "Curvature [m⁻¹]", data.profile.segments);
        auto profile_view = new ProfileView(data);
        QObject::connect(series_view, &SeriesView::selectionChanged, profile_view, &ProfileView::setSelection);

        auto hbox0 = new QHBoxLayout();
        hbox0->addStretch();
        hbox0->addWidget(new QLabel("X0 [m]:")); hbox0->addSpacing(10); hbox0->addWidget(new DoubleView(data.profile.x0));
        hbox0->addSpacing(10);
        hbox0->addStretch();
        hbox0->addWidget(new QLabel("Y0 [m]:")); hbox0->addSpacing(10); hbox0->addWidget(new DoubleView(data.profile.y0));
        hbox0->addSpacing(10);
        hbox0->addStretch();
        hbox0->addWidget(new QLabel("Angle [rad]:")); hbox0->addSpacing(10); hbox0->addWidget(new DoubleView(data.profile.phi0));
        hbox0->addStretch();

        auto offsets = new QGroupBox("Offsets");
        offsets->setLayout(hbox0);

        auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
        QObject::connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);

        // Layout

        auto vbox0 = new QVBoxLayout();
        vbox0->addWidget(offsets);
        vbox0->addWidget(profile_view, 1);

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
