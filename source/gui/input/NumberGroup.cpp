#include "NumberGroup.hpp"
#include "gui/views/DoubleView.hpp"
#include "gui/views/IntegerView.hpp"
#include "model/InputData.hpp"

NumberGroup::NumberGroup(const QString& title)
    : QGroupBox(title),
      grid(new QGridLayout())
{
    grid->setAlignment(Qt::AlignTop);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 0);
    grid->setColumnStretch(2, 0);

    this->setLayout(grid);
}

void NumberGroup::addRow(const QString& name, const QString& unit, DocumentItem<double>& item)
{
    addRow(name, unit, new DoubleView(item));
}

void NumberGroup::addRow(const QString& name, const QString& unit, DocumentItem<int>& item)
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
