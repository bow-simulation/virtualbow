#pragma once
#include "SeriesView.hpp"
#include "NumberGroup.hpp"
#include "StringView.hpp"
#include "Plot.hpp"
#include "../model/InputData.hpp"

class ProfileEditor: public QWidget
{
public:
    ProfileEditor(InputData& data)
    {
        auto hbox = new QHBoxLayout();
        this->setLayout(hbox);

        auto vbox = new QVBoxLayout();
        hbox->addLayout(vbox);

        auto series_view = new SeriesView("Length", "Curvature", data.profile_curvature);
        vbox->addWidget(series_view);

        auto group_limb = new NumberGroup(data, "Offset");
        group_limb->addRow("x:", data.profile_offset_x);
        group_limb->addRow("y:", data.profile_offset_y);
        group_limb->addRow("Angle:", data.profile_angle);
        vbox->addWidget(group_limb);

        auto plot = new Plot();
        hbox->addWidget(plot, 1);
    }
};

class WidthEditor: public QWidget
{
public:
    WidthEditor(InputData& data)
    {
        auto hbox = new QHBoxLayout();
        this->setLayout(hbox);

        auto series_view = new SeriesView("Position", "Width", data.sections_width);
        hbox->addWidget(series_view);

        auto plot = new Plot();
        hbox->addWidget(plot, 1);
    }
};

class HeightEditor: public QWidget
{
public:
    HeightEditor(InputData& data)
    {
        auto hbox = new QHBoxLayout();
        this->setLayout(hbox);

        auto series_view = new SeriesView("Position", "Height", data.sections_height);
        hbox->addWidget(series_view);

        auto plot = new Plot();
        hbox->addWidget(plot, 1);
    }
};

class BowEditor: public QWidget
{
public:
    BowEditor(InputData& data)
    {
        auto vbox = new QVBoxLayout();
        this->setLayout(vbox);

        auto hbox = new QHBoxLayout();
        vbox->addLayout(hbox);

        // Limb group
        auto group_limb = new NumberGroup(data, "Limb Material");
        group_limb->addRow("rho:", data.sections_rho);      // Todo: Use unicode character
        group_limb->addRow("E:", data.sections_E);
        hbox->addWidget(group_limb);

        // String group
        auto group_string = new NumberGroup(data, "String");
        group_string->addRow("Strand stiffness:", data.string_strand_stiffness);
        group_string->addRow("Strand density:", data.string_strand_density);
        group_string->addRow("Number of strands:", data.string_n_strands);
        hbox->addWidget(group_string);

        // Operation group
        auto group_operation = new NumberGroup(data, "Operation");
        group_operation->addRow("Brace height:", data.operation_brace_height);
        group_operation->addRow("Draw length:", data.operation_draw_length);
        group_operation->addRow("Arrow mass:", data.operation_mass_arrow);
        hbox->addWidget(group_operation);

        // Masses group
        auto group_masses = new NumberGroup(data, "Additional masses");
        group_masses->addRow("String center:", data.mass_string_center);
        group_masses->addRow("String tip:", data.mass_string_tip);
        group_masses->addRow("Limb tip:", data.mass_limb_tip);
        hbox->addWidget(group_masses);

        // Limb geometry editors
        auto tabs = new QTabWidget();
        tabs->addTab(new ProfileEditor(data), "Profile");
        tabs->addTab(new WidthEditor(data), "Width");
        tabs->addTab(new HeightEditor(data), "Height");
        vbox->addWidget(tabs, 1);

        /*
        // Comments
        auto view_comments = new StringView(data.meta_comments);
        view_comments->setFixedHeight(120);     // Todo: Magic number
        auto box_comments = new QHBoxLayout();
        auto group_comments = new QGroupBox("Comments");
        box_comments->addWidget(view_comments);
        group_comments->setLayout(box_comments);
        vbox->addWidget(group_comments);
        */
    }
};
