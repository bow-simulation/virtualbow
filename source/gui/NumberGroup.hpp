#pragma once
#include "DoubleView.hpp"
#include "IntegerView.hpp"
#include "../model/InputData.hpp"
#include <QtWidgets>

class NumberGroup: public QGroupBox
{
public:
    NumberGroup(InputData& data, const QString& title)
        : QGroupBox(title),
          vbox(new QVBoxLayout)
    {
        this->setLayout(vbox);
    }

    void addRow(const QString& lb, DocItem<double>& item)
    {
        addRow(lb, new DoubleView(item));
    }

    void addRow(const QString& lb, DocItem<int>& item)
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

    QVBoxLayout* vbox;
};
