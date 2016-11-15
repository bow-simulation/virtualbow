#pragma once
#include <QtWidgets>

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
