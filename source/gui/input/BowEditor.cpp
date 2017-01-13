#include "BowEditor.hpp"
#include "SeriesView.hpp"
#include "NumberGroup.hpp"
#include "LimbViews.hpp"
#include "../../model/InputData.hpp"

BowEditor::BowEditor(InputData& data)
    : QSplitter(Qt::Vertical)
{
    this->addWidget(new ProfileView(data));

    auto tabs = new QTabWidget();
    tabs->addTab(new GeneralEditor(data), "General");
    tabs->addTab(new ProfileEditor(data), "Profile");
    tabs->addTab(new WidthEditor(data), "Width");
    tabs->addTab(new HeightEditor(data), "Layers");
    this->addWidget(tabs);

    this->setStretchFactor(1, 0);
}

GeneralEditor::GeneralEditor(InputData& data)
{
    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);

    auto hbox = new QHBoxLayout();
    vbox->addLayout(hbox, 0);
    vbox->addStretch();

    auto group_limb = new NumberGroup(data, "Limb Material");
    group_limb->addRow("rho:", "kg/m³", data.sections_rho);      // Todo: Use unicode character (\u2374). Problem: Windows
    group_limb->addRow("E:", "N/m²", data.sections_E);
    hbox->addWidget(group_limb);

    auto group_string = new NumberGroup(data, "String");
    group_string->addRow("Strand stiffness:", "N/100%", data.string_strand_stiffness);
    group_string->addRow("Strand density:", "kg/m", data.string_strand_density);
    group_string->addRow("Number of strands:", "", data.string_n_strands);
    hbox->addWidget(group_string);

    auto group_masses = new NumberGroup(data, "Additional masses");
    group_masses->addRow("String center:", "kg", data.mass_string_center);
    group_masses->addRow("String tip:", "kg", data.mass_string_tip);
    group_masses->addRow("Limb tip:", "kg", data.mass_limb_tip);
    hbox->addWidget(group_masses);

    auto group_operation = new NumberGroup(data, "Operation");
    group_operation->addRow("Brace height:", "m", data.operation_brace_height);
    group_operation->addRow("Draw length:", "m", data.operation_draw_length);
    group_operation->addRow("Arrow mass:", "kg", data.operation_mass_arrow);
    hbox->addWidget(group_operation);
}

ProfileEditor::ProfileEditor(InputData& data)
{
    auto hbox = new QHBoxLayout();
    this->setLayout(hbox);

    auto vbox = new QVBoxLayout();
    hbox->addLayout(vbox);

    auto series_view = new SeriesView("Length [m]", "Curvature [m⁻¹]", data.profile_segments);
    vbox->addWidget(series_view);

    auto group_limb = new NumberGroup(data, "Offset");
    group_limb->addRow("x:", "m", data.profile_x0);
    group_limb->addRow("y:", "m", data.profile_y0);
    group_limb->addRow("Angle:", "rad", data.profile_phi0);
    vbox->addWidget(group_limb);

    auto plot = new ProfileView(data);
    hbox->addWidget(plot, 1);
}

WidthEditor::WidthEditor(InputData& data)
{
    auto hbox = new QHBoxLayout();
    this->setLayout(hbox);

    auto series_view = new SeriesView("Position", "Width [m]", data.sections_width);
    hbox->addWidget(series_view);

    auto plot = new SplineView("Position", "Width [m]", data.sections_width);
    hbox->addWidget(plot, 1);
}


HeightEditor::HeightEditor(InputData& data)
{
    auto hbox = new QHBoxLayout();
    this->setLayout(hbox);

    auto series_view = new SeriesView("Position", "Height [m]", data.sections_height);
    hbox->addWidget(series_view);

    auto plot = new SplineView("Position", "Height [m]", data.sections_height);
    hbox->addWidget(plot, 1);
}


