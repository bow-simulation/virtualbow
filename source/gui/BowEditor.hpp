#pragma once
#include "DoubleView.hpp"
#include "IntegerView.hpp"
#include "StringView.hpp"
#include "BowPreview.hpp"

#include "../numerics/StepFunction.hpp"
#include "../model/InputData.hpp"


// Todo: Rename InputGroup, methods addDouble and addInteger
class DoubleGroup: public QGroupBox
{
public:
    DoubleGroup(InputData& data, const QString& title)
        : QGroupBox(title),
          data(data),
          vbox(new QVBoxLayout)
    {
        this->setLayout(vbox);
    }

    template<DomainTag D>
    void addRow(const QString& lb, DocItem<double>& item)
    {
        addRow(lb, new DoubleView<D>(item));
    }

    void addRow(const QString& lb, DocItem<unsigned>& item)
    {
        addRow(lb, new IntegerView(item));
    }

private:
    void addRow(const QString& lb, QWidget* field)
    {
        auto label = new QLabel(lb);
        label->setAlignment(Qt::AlignRight);

        auto hbox = new QHBoxLayout();
        vbox->addLayout(hbox);
        hbox->addWidget(label, 1);
        hbox->addWidget(field, 0);
    }

    InputData& data;
    QVBoxLayout* vbox;
};

class BowEditor: public QWidget
{
public:
    BowEditor(InputData& data)
    {
        auto vbox = new QVBoxLayout();
        this->setLayout(vbox);

        auto tabs = new QTabWidget();
        tabs->addTab(new QWidget(),"Profile");
        tabs->addTab(new QWidget(),"Width");
        tabs->addTab(new QWidget(),"Height");
        vbox->addWidget(tabs, 1);

        auto hbox = new QHBoxLayout();
        vbox->addLayout(hbox);

        // Material
        auto group_material = new DoubleGroup(data, "Limb material");
        group_material->addRow<DomainTag::Pos>("rho:", data.sections_rho);
        group_material->addRow<DomainTag::Pos>("E:", data.sections_E);
        hbox->addWidget(group_material);

        // String
        auto group_string = new DoubleGroup(data, "String");
        group_string->addRow<DomainTag::Pos>("Strand stiffness:", data.string_strand_stiffness);
        group_string->addRow<DomainTag::Pos>("Strand density:", data.string_strand_density);
        group_string->addRow<DomainTag::Pos>("Number of strands:", data.string_n_strands);
        hbox->addWidget(group_string);

        // Operation
        auto group_operation = new DoubleGroup(data, "Operation");
        group_operation->addRow<DomainTag::Pos>("Brace height:", data.operation_brace_height);
        group_operation->addRow<DomainTag::Pos>("Draw length:", data.operation_draw_length);
        group_operation->addRow<DomainTag::Pos>("Arrow mass:", data.operation_mass_arrow);
        hbox->addWidget(group_operation);

        // Masses
        auto group_masses = new DoubleGroup(data, "Additional masses");
        group_masses->addRow<DomainTag::NonNeg>("String center:", data.mass_string_center);
        group_masses->addRow<DomainTag::NonNeg>("String tip:", data.mass_string_tip);
        group_masses->addRow<DomainTag::NonNeg>("Limb tip:", data.mass_limb_tip);
        hbox->addWidget(group_masses);
        //hbox->addStretch(1);

        // Comments
        auto view_comments = new StringView(data.meta_comments);
        auto box_comments = new QHBoxLayout();
        auto group_comments = new QGroupBox("Comments");
        box_comments->addWidget(view_comments);
        group_comments->setLayout(box_comments);
        vbox->addWidget(group_comments);
    }
};
