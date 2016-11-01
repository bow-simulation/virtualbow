#pragma once
#include "SeriesView.hpp"
#include "NumberGroup.hpp"
#include "StringView.hpp"
#include "../model/InputData.hpp"

class BowEditor: public QWidget
{
public:
    BowEditor(InputData& data)
    {
        auto vbox = new QVBoxLayout();
        this->setLayout(vbox);

        auto tabs = new QTabWidget();
        tabs->addTab(new SeriesView("Length", "Curvature", data.profile_curvature), "Profile");
        tabs->addTab(new QWidget(),"Width");
        tabs->addTab(new QWidget(),"Height");
        vbox->addWidget(tabs, 1);

        auto hbox = new QHBoxLayout();
        vbox->addLayout(hbox);

        // Material group
        auto group_material = new NumberGroup(data, "Limb material");
        group_material->addRow("rho:", data.sections_rho);
        group_material->addRow("E:", data.sections_E);
        hbox->addWidget(group_material);

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

        // Comments
        auto view_comments = new StringView(data.meta_comments);
        view_comments->setFixedHeight(120);     // Todo: Magic number
        auto box_comments = new QHBoxLayout();
        auto group_comments = new QGroupBox("Comments");
        box_comments->addWidget(view_comments);
        group_comments->setLayout(box_comments);
        vbox->addWidget(group_comments);
    }
};
