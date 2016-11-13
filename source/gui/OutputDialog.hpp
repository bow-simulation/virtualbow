#pragma once
#include "../model/OutputData.hpp"
#include <QtWidgets>

#include "Plot.hpp"

class OutputGrid: public QWidget
{
public:
    OutputGrid()
        : grid(new QGridLayout())
    {
        auto hbox = new QHBoxLayout();
        this->setLayout(hbox);
        hbox->addLayout(grid);
        hbox->addStretch();

        grid->setHorizontalSpacing(15);     // Todo: Unify with NumberGroup. Todo: Spacing only after labels.
    }

    void add(int i, int j, const QString& text, double value)
    {
        auto label = new QLabel(text);
        auto edit = new QLineEdit(QLocale::c().toString(value));
        edit->setReadOnly(true);

        grid->addWidget(label, i, 2*j, 1, 1, Qt::AlignRight);
        grid->addWidget(edit, i, 2*j+1);
        grid->setColumnStretch(2*j, 1);
    }

private:
    QGridLayout* grid;
};

class StaticOutput: public QWidget
{
public:
    StaticOutput(const BowStates& statics)
    {
        auto vbox = new QVBoxLayout();
        this->setLayout(vbox);

        auto grid = new OutputGrid();
        grid->add(0, 0, "String length:", 0.0);
        grid->add(1, 0, "Draw force:", 0.0);
        grid->add(0, 1, "Energy braced:", 0.0);
        grid->add(1, 1, "Energy drawn:", 0.0);
        grid->add(0, 2, "Drawing work:", 0.0);
        grid->add(1, 2, "Storage ratio:", 0.0);
        vbox->addWidget(grid);

        auto plot = new Plot("Draw length", "Draw force");
        plot->addSeries({statics.pos_string_center, statics.draw_force});

        auto tabs = new QTabWidget();
        tabs->addTab(plot, "Draw characteristics");
        tabs->addTab(new QWidget(), "Shapes");
        tabs->addTab(new QWidget(), "Stresses");
        vbox->addWidget(tabs);
    }
};

class DynamicOutput: public QWidget
{
public:
    DynamicOutput(const BowStates& dynamics)
    {
        auto hbox = new QHBoxLayout();
        this->setLayout(hbox);

        auto tabs = new QTabWidget();
        tabs->addTab(new QWidget(), "Shot characteristics");
        tabs->addTab(new QWidget(), "Shapes");
        tabs->addTab(new QWidget(), "Stresses");
        hbox->addWidget(tabs);
    }
};

class OutputDialog: public QDialog
{
public:
    OutputDialog(QWidget* parent, const OutputData& output)
        : QDialog(parent)
    {
        auto vbox = new QVBoxLayout();
        this->setLayout(vbox);
        this->setWindowTitle("Simulation results");
        this->resize(900, 700);     // Todo: Magic numbers

        auto tabs = new QTabWidget();
        tabs->addTab(output.statics ? new StaticOutput(*output.statics) : new QWidget(), "Statics");
        tabs->addTab(output.dynamics ? new DynamicOutput(*output.dynamics) : new QWidget(), "Dynamics");
        tabs->setTabEnabled(0, output.statics != nullptr);
        tabs->setTabEnabled(1, output.dynamics != nullptr);
        tabs->setIconSize({24, 24});    // Todo: Magic numbers
        vbox->addWidget(tabs, 1);

        tabs->setDocumentMode(true);

        // Todo: Better solution?
        // When a tab is set as disabled the icons are somehow converted to grayscale internally.
        // So maybe there is no need to have an extra grey icon. Look at Qt source how they do it.
        auto set_icons = [tabs](int index)
        {
            switch(index)
            {
            case 0:
                tabs->setTabIcon(0, QIcon(":/icons/circle-yellow"));
                tabs->setTabIcon(1, QIcon(":/icons/circle-grey"));
                break;
            case 1:
                tabs->setTabIcon(0, QIcon(":/icons/circle-grey"));
                tabs->setTabIcon(1, QIcon(":/icons/circle-green"));
                break;
            }
        };

        QObject::connect(tabs, &QTabWidget::currentChanged, set_icons);
        set_icons(0);

        auto btbox = new QDialogButtonBox(QDialogButtonBox::Close);
        QObject::connect(btbox, &QDialogButtonBox::rejected, this, &QDialog::close);
        vbox->addWidget(btbox);
    }
};
