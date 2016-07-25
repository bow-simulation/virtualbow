#pragma once
#include "../model/Limb.hpp"

#include <QtWidgets>



class BowEditor: public QWidget
{
    Q_OBJECT

public:
    BowEditor()
    {
        QPushButton* bt_curvature = new QPushButton("Edit");
        QLineEdit* tf_offset = new QLineEdit();
        QLineEdit* tf_angle = new QLineEdit();

        QFormLayout *form = new QFormLayout;
        form->addRow("Curvature:", bt_curvature);
        form->addRow("Offset:", tf_offset);
        form->addRow("Angle:", tf_angle);
        this->setLayout(form);

        QGroupBox* box = new QGroupBox("Limb Geometry");
        box->setLayout(form);

        QHBoxLayout* h = new QHBoxLayout();
        h->addWidget(box);
        this->setLayout(h);
    }

private:
    Limb::Parameters data;

};
