#pragma once
#include "NumberView.hpp"

struct Parameters
{
    double angle = 3.14;
    double offset = 2.17;
};

class BowEditor: public QWidget
{
    Q_OBJECT

public:
    BowEditor()
    {
        //auto validator = new RealValidator<Domain::Pos>();
        //tf_offset->setValidator(validator);

        auto tf_angle = new NumberView<double, Domain::All>(param.angle);
        auto tf_offset = new NumberView<double, Domain::NonNeg>(param.offset);

        auto form = new QFormLayout();
        form->addRow("Angle:", tf_angle);
        form->addRow("Offset:", tf_offset);
        this->setLayout(form);

        auto box = new QGroupBox("Limb Geometry");
        box->setLayout(form);

        auto h = new QHBoxLayout();
        h->addWidget(box);
        this->setLayout(h);
    }

private:
    Parameters param;
};
