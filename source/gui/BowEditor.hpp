#pragma once
#include "DoubleView.hpp"
#include "TextView.hpp"
#include "DoubleEditor.hpp"
#include "SeriesView.hpp"
#include "SeriesEditor.hpp"
#include "Document.hpp"
#include "BowPreview.hpp"

#include "../numerics/StepFunction.hpp"
#include "../model/InputData.hpp"


class DoubleGroup: public QGroupBox
{
public:
    DoubleGroup(Document& doc, const QString& title)
        : QGroupBox(title),
          doc(doc),
          layout(new QFormLayout)
    {
        this->setLayout(layout);
    }

    template<DomainTag D>
    void addRow(const QString& label, std::function<double&(InputData&)> f)
    {
        auto field = new DoubleView<D>(DocumentItem<double>(doc, f));
        layout->addRow(label, field);
    }

private:
    Document& doc;
    QFormLayout* layout;
};

class BowEditor: public QWidget
{
public:
    BowEditor(Document& doc)
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

        // String
        auto group_string = new DoubleGroup(doc, "String");
        group_string->addRow<DomainTag::Pos>("Strand stiffness:", [](InputData& input)->double&{ return input.string.strand_stiffness; });
        group_string->addRow<DomainTag::Pos>("Strand density:", [](InputData& input)->double&{ return input.string.strand_density; });
        group_string->addRow<DomainTag::Pos>("Number of strands:", [](InputData& input)->double&{ return input.string.n_strands; });
        hbox->addWidget(group_string);

        // Operation
        auto group_operation = new DoubleGroup(doc, "Operation");
        group_operation->addRow<DomainTag::Pos>("Brace height:", [](InputData& input)->double&{ return input.operation.brace_height; });
        group_operation->addRow<DomainTag::Pos>("Draw length:", [](InputData& input)->double&{ return input.operation.draw_length; });
        group_operation->addRow<DomainTag::Pos>("Arrow mass:", [](InputData& input)->double&{ return input.operation.arrow_mass; });
        hbox->addWidget(group_operation);

        // Masses
        auto group_masses = new DoubleGroup(doc, "Additional masses");
        group_masses->addRow<DomainTag::NonNeg>("String center:", [](InputData& input)->double&{ return input.masses.string_center; });
        group_masses->addRow<DomainTag::NonNeg>("String tip:", [](InputData& input)->double&{ return input.masses.string_tip; });
        group_masses->addRow<DomainTag::NonNeg>("Limb tip:", [](InputData& input)->double&{ return input.masses.limb_tip; });
        hbox->addWidget(group_masses);
        hbox->addStretch(1);

        // Comments
        auto view_comments = new TextView(DocumentItem<std::string>(doc, [](InputData& input)->std::string&{ return input.meta.comments; }));
        auto box_comments = new QHBoxLayout();
        auto group_comments = new QGroupBox("Comments");
        box_comments->addWidget(view_comments);
        group_comments->setLayout(box_comments);
        vbox->addWidget(group_comments);
    }
};
