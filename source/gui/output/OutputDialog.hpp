#pragma once
#include "../../model/OutputData.hpp"
#include <QtWidgets>

#include "../Plot.hpp"

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

        grid->setHorizontalSpacing(15);     // Todo: Unify with NumberGroup. Todo: Spacing only after line edits.
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

class ShapePlot: public QWidget
{
public:
    ShapePlot(const BowSetup& setup, const BowStates& states, const std::vector<double>& parameter, const QString& text)
        : states(states),
          parameter(parameter)
    {
        auto vbox = new QVBoxLayout();
        this->setLayout(vbox);

        plot = new Plot("x", "y");
        plot->fixAspectRatio(true);
        vbox->addWidget(plot);

        plot->addSeries({setup.limb.x, setup.limb.y});
        plot->setLinePen(0, QPen(QBrush(Qt::lightGray), 2));

        // "Stroboscope" plots during different draw lengths
        unsigned steps = 2; // Todo: Magic number
        for(unsigned i = 0; i <= steps; ++i)
        {
            size_t j = i*(parameter.size()-1)/steps;
            plot->addSeries({states.pos_limb_x[j], states.pos_limb_y[j]});
            plot->addSeries({states.pos_string_x[j], states.pos_string_y[j]});
            plot->setLinePen(plot->count()-2, QPen(QBrush(Qt::lightGray), 2));
            plot->setLinePen(plot->count()-1, QPen(QBrush(Qt::lightGray), 1));
        }

        // Plot at user defined draw length
        plot->addSeries();
        plot->addSeries();
        plot->setLinePen(plot->count()-2, QPen(QBrush(Qt::blue), 2));
        plot->setLinePen(plot->count()-1, QPen(QBrush(Qt::blue), 1));

        auto hbox = new QHBoxLayout();
        vbox->addLayout(hbox);
        hbox->addSpacing(10);   // Todo: Magic number // Todo: Remove the spacing around the plot instead of adding these
        hbox->addWidget(new QLabel(text));

        auto edit = new QLineEdit();
        auto validator = new QDoubleValidator(parameter.front(), parameter.back(), 10); // Todo: Magic number
        validator->setLocale(QLocale::c());
        edit->setValidator(validator);
        hbox->addWidget(edit);
        hbox->addSpacing(15);   // Todo: Magic number

        auto slider = new QSlider(Qt::Horizontal);
        slider->setRange(0, parameter.size()-1);
        hbox->addWidget(slider, 1);
        hbox->addSpacing(10);   // Todo: Magic number // Todo: Remove the spacing around the plot instead of adding these

        // Event handling

        QObject::connect(slider, &QSlider::valueChanged, [this, edit](int index)
        {
            setParameterIndex(index);
            edit->setText(QLocale::c().toString(this->parameter[index]));
        });

        QObject::connect(edit, &QLineEdit::editingFinished, [this, edit, slider]()
        {
            double value = QLocale::c().toDouble(edit->text());
            double min = this->parameter.front();
            double max = this->parameter.back();

            double p = (value - min)/(max - min);
            slider->setValue(double(slider->minimum())*(1.0 - p) + double(slider->maximum())*p);
        });

        emit slider->valueChanged(0);
    }

private:
    const BowStates& states;
    const std::vector<double>& parameter;

    Plot* plot;

    void setParameterIndex(int index)
    {
        plot->setData(plot->count()-2, {states.pos_limb_x[index], states.pos_limb_y[index]});
        plot->setData(plot->count()-1, {states.pos_string_x[index], states.pos_string_y[index]});
        plot->replot();
    }
};

class StaticOutput: public QWidget
{
public:
    StaticOutput(const BowSetup& setup, const BowStates& statics)
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
        tabs->addTab(new ShapePlot(setup, statics, statics.pos_string_center, "Draw length:"), "Shapes");
        tabs->addTab(new QWidget(), "Stresses");
        vbox->addWidget(tabs);
    }
};

class DynamicOutput: public QWidget
{
public:
    DynamicOutput(const BowSetup& setup, const BowStates& dynamics)
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
        this->resize(900, 800);     // Todo: Magic numbers

        auto tabs = new QTabWidget();
        tabs->addTab(output.statics ? new StaticOutput(output.setup, *output.statics) : new QWidget(), "Statics");
        tabs->addTab(output.dynamics ? new DynamicOutput(output.setup, *output.dynamics) : new QWidget(), "Dynamics");
        tabs->setTabEnabled(0, output.statics != nullptr);
        tabs->setTabEnabled(1, output.dynamics != nullptr);
        tabs->setDocumentMode(true);
        tabs->setIconSize({24, 24});    // Todo: Magic numbers
        vbox->addWidget(tabs, 1);

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

private:
    // Keep dialog from closing on enter
    virtual void keyPressEvent(QKeyEvent *evt) override
    {
        if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
            return;
        QDialog::keyPressEvent(evt);
    }
};
