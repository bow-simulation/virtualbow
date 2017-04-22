#pragma once
#include "gui/input/DoubleView.hpp"
#include "gui/input/IntegerView.hpp"

#include <QtWidgets>

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

        vbox->insertWidget(vbox->count()-1, group);
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

        int row = grid->rowCount();
        grid->addWidget(new QLabel(name), row, 0, 1, 1, Qt::AlignRight);
        grid->addWidget(new QLabel(unit.isEmpty() ? "" : "[" + unit + "]"), row, 2, 1, 1, Qt::AlignLeft);
        grid->addWidget(field, row, 1);
    }

private:
    QVBoxLayout* vbox;
    QGridLayout* grid;
    QGroupBox* group;
};
