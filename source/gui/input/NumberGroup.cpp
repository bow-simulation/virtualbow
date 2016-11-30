#include "NumberGroup.hpp"
#include "DoubleView.hpp"
#include "IntegerView.hpp"
#include "../../model/InputData.hpp"

NumberGroup::NumberGroup(InputData& data, const QString& title)
    : QGroupBox(title),
      grid(new QGridLayout())
{
    grid->setColumnStretch(0, 0);
    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(2, 0);

    auto vbox = new QVBoxLayout();
    vbox->addLayout(grid);
    vbox->addStretch(1);

    this->setLayout(vbox);
}

void NumberGroup::addRow(const QString& name, const QString& unit, DocItem<double>& item)
{
    addRow(name, unit, new DoubleView(item));
}

void NumberGroup::addRow(const QString& name, const QString& unit, DocItem<int>& item)
{
    addRow(name, unit, new IntegerView(item));
}

void NumberGroup::addRow(const QString& name, const QString& unit, QWidget* field)
{
    int row = grid->rowCount();
    grid->addWidget(new QLabel(name), row, 0, 1, 1, Qt::AlignRight);
    grid->addWidget(new QLabel(unit.isEmpty() ? "" : "[" + unit + "]"), row, 2, 1, 1, Qt::AlignLeft);
    grid->addWidget(field, row, 1);
}
