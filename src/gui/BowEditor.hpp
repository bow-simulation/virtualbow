#pragma once
#include "NumberView.hpp"
#include "SeriesEditor.hpp"
#include "Document.hpp"

#include "../numerics/StepFunction.hpp"
#include "../model/InputData.hpp"

class BowEditor: public QWidget
{
    Q_OBJECT

public:
    BowEditor()
    {
        /*
        auto bt_width = new QPushButton("Edit");
        auto bt_height = new QPushButton("Edit");

        auto bt_curvature = new QPushButton("Edit");
        connect(bt_curvature, &QPushButton::clicked, [&]()
        {
            auto edit = new SeriesEditor(this, param.curvature, [](const DataSeries& input)
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

            edit->setInputLabels("Width", "Curvature");
            edit->setOutputLabels("Arc length", "Curvature");
            edit->exec();
        });
        */

        auto tf_offset_x = new NumberView<double>(DocumentItem<double>(doc, [](InputData& input)->double&{ return input.limb.offset_x; }));
        auto tf_offset_y = new NumberView<double>(DocumentItem<double>(doc, [](InputData& input)->double&{ return input.limb.offset_y; }));
        auto tf_angle    = new NumberView<double>(DocumentItem<double>(doc, [](InputData& input)->double&{ return input.limb.angle;    }));

        auto form = new QFormLayout();
        //form->addRow("Curvature", bt_curvature);
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

private:
    Document doc;
};
