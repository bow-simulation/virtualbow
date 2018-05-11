#include "OutputGrid.hpp"

OutputGrid::OutputGrid()
    : grid(new QGridLayout())
{
    auto vbox = new QVBoxLayout();
    vbox->addLayout(grid);
    vbox->addStretch();

    auto hbox = new QHBoxLayout();
    this->setLayout(hbox);
    hbox->addLayout(vbox);
    hbox->addStretch();

    grid->setMargin(20);
    grid->setHorizontalSpacing(15);                      // Todo: Spacing only after line edits
}

void OutputGrid::add(int i, int j, const QString& text, double value)
{
    auto edit = new QLineEdit(QLocale().toString(value));
    edit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    edit->setReadOnly(true);

    auto label = new QLabel(text);
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    grid->addWidget(label, i, 2*j, Qt::AlignRight);
    grid->addWidget(edit, i, 2*j+1);
    grid->setColumnStretch(2*j, 0);
    grid->setColumnStretch(2*j+1, 0);
}
