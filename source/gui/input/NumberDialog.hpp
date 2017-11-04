#pragma once
#include "gui/input/DoubleView.hpp"
#include "gui/input/IntegerView.hpp"

#include <QtWidgets>

class WidthGroup
{
public:
    void addWidget(QWidget* widget)
    {
        widgets.push_back(widget);
        width = std::max(width, widget->sizeHint().width());

        for(auto w: widgets)
            w->setMinimumWidth(width);
    }

private:
    std::vector<QWidget*> widgets;
    int width = 0;
};

class NumberDialog: public QDialog
{
public:
    NumberDialog(QWidget* parent, const QString& title)
        : QDialog(parent),
          vbox(new QVBoxLayout()),
          grid(nullptr),
          group(nullptr)
    {
        auto btbox = new QDialogButtonBox(QDialogButtonBox::Ok);
        QObject::connect(btbox, &QDialogButtonBox::accepted, this, &QDialog::accept);
        vbox->addWidget(btbox, 0, Qt::AlignBottom);

        this->setWindowTitle(title);
        this->setLayout(vbox);
    }

    void addGroup(const QString& name)
    {
        grid = new QGridLayout();
        grid->setAlignment(Qt::AlignTop);
        grid->setColumnStretch(0, 1);
        grid->setColumnStretch(1, 0);
        grid->setColumnStretch(2, 0);

        group = new QGroupBox(name);
        group->setLayout(grid);

        vbox->insertWidget(vbox->count()-1, group, 1);
    }

    void addField(const QString& name, const QString& unit, DocItem<double>& item)
    {
        addField(name, unit, new DoubleView(item));
    }

    void addField(const QString& name, const QString& unit, DocItem<int>& item)
    {
        addField(name, unit, new IntegerView(item));
    }

    void addField(const QString& name, const QString& unit, QWidget* field)
    {
        if(!group)
        {
            addGroup("");
        }

        QLabel* lb_name = new QLabel(name);
        lb_name->setAlignment(Qt::AlignRight);

        QLabel* lb_unit = new QLabel(unit.isEmpty() ? "" : "[" + unit + "]");
        lb_unit->setAlignment(Qt::AlignLeft);

        int row = grid->rowCount();
        grid->addWidget(lb_name, row, 0);
        grid->addWidget(field, row, 1);
        grid->addWidget(lb_unit, row, 2);

        width_group_0.addWidget(lb_name);
        width_group_1.addWidget(field);
        width_group_2.addWidget(lb_unit);
    }

private:
    WidthGroup width_group_0;
    WidthGroup width_group_1;
    WidthGroup width_group_2;

    QVBoxLayout* vbox;
    QGridLayout* grid;
    QGroupBox* group;
};
