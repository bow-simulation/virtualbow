#pragma once
#include "ScalarView.hpp"
#include "DoubleEditor.hpp"
#include "SeriesView.hpp"
#include "SeriesEditor.hpp"
#include "Document.hpp"

#include "../numerics/StepFunction.hpp"
#include "../model/InputData.hpp"

class BowEditor: public QWidget
{
    Q_OBJECT

public:
    BowEditor(Document& document)
    {
        auto bt_curvature = new QPushButton("Edit");
        QObject::connect(bt_curvature, &QPushButton::clicked, [&]()
        {
            auto edit = new SeriesEditor(this, [](const DataSeries& input)
            {
                try
                {
                    StepFunction f(input);
                    return f.sample();
                }
                catch(std::runtime_error e)
                {
                    return DataSeries();
                }
            });

            edit->setWindowTitle("Edit profile");
            edit->setInputLabels("Seg. length", "Curvature");

            DocumentItem<DataSeries> doc_item(document, [](InputData& input)->DataSeries&{ return input.limb.curvature; });
            edit->setData(doc_item.getData());
            edit->setOutputLabels("Arc length", "Curvature");

            if(edit->exec() == QDialog::Accepted)
            {
                doc_item.setData(edit->getData());
            }
        });

        /*
        auto tf_offset_x = new ScalarView<double>(DocumentItem<double>(document, [](InputData& input)->double&{ return input.limb.offset_x; }));
        auto tf_offset_y = new ScalarView<double>(DocumentItem<double>(document, [](InputData& input)->double&{ return input.limb.offset_y; }));
        auto tf_angle    = new ScalarView<double>(DocumentItem<double>(document, [](InputData& input)->double&{ return input.limb.angle;    }));
        */

        auto tf_offset_x = new DoubleEditor<DomainTag::All>();
        auto tf_offset_y = new DoubleEditor<DomainTag::Pos>();
        auto tf_angle    = new DoubleEditor<DomainTag::Neg>();

        QObject::connect(tf_offset_x, &DoubleEditor<DomainTag::All>::valueChanged, [](double value)
        {
            qInfo() << "New value: " << value;
        });

        auto form = new QFormLayout();
        form->addRow("Curvature", bt_curvature);
        form->addRow("Offset x:", tf_offset_x);
        form->addRow("Offset y:", tf_offset_y);
        form->addRow("Angle:", tf_angle);
        this->setLayout(form);

        auto box = new QGroupBox("Profile");
        box->setLayout(form);

        auto h = new QHBoxLayout();
        h->addWidget(box);
        this->setLayout(h);
    }
};
