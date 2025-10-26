#include "NumberGrid.hpp"
#include "pre/widgets/DoubleOutput.hpp"
#include "pre/models/units/Quantity.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>

NumberGrid::NumberGrid()
    : columnLayout(new QHBoxLayout()),
      currentColumn(nullptr),
      currentGrid(nullptr)
{
    columnLayout->setSpacing(20);
    columnLayout->setContentsMargins(20, 20, 20, 20);
    columnLayout->addStretch();
    this->setLayout(columnLayout);
}

void NumberGrid::addColumn() {
    // Create new column
    currentColumn = new QVBoxLayout();
    currentColumn->addStretch();

    // Forget old grid
    currentGrid = nullptr;

    // Insert before horizontal stretch
    int i = columnLayout->count() - 1;
    columnLayout->insertLayout(i, currentColumn);
}

void NumberGrid::addGroup(const QString& name) {
    if(currentColumn == nullptr) {
        addColumn();
    }

    currentGrid = new QGridLayout();
    auto group = new QGroupBox(name);
    group->setLayout(currentGrid);

    // Insert before vertical stretch
    int i = currentColumn->count() - 1;
    currentColumn->insertWidget(i, group);
}

void NumberGrid::addHeaders(const QStringList& headers) {
    if(currentColumn == nullptr) {
        addColumn();
    }

    if(currentGrid == nullptr) {
        addGroup("Default");
    }

    int row = currentGrid->rowCount();

    for(int col = 0; col < headers.size(); ++col) {
        auto label = new QLabel(headers[col]);
        currentGrid->addWidget(label, row, col + 1, Qt::AlignCenter);
    }
}

void NumberGrid::addValues(const QString& name, const QList<double>& values, const QList<const Quantity*> quantities, const QList<double>& allowed, const QList<double>& maximum, int decimals) {
    if(currentColumn == nullptr) {
        addColumn();
    }

    if(currentGrid == nullptr) {
        addGroup("Default");
    }

    int row = currentGrid->rowCount();
    auto label = new QLabel(name);
    currentGrid->addWidget(label, row, 0);

    for(int col = 0; col < values.size(); ++col) {
        auto output = new DoubleOutput(values[col], *quantities[col], decimals);
        currentGrid->addWidget(output, row, col + 1);

        // Set warning limits if nonzero ones are given
        if(col < allowed.size() && col < maximum.size() && allowed[col] != 0.0 && maximum[col] != 0.0) {
            output->setAllowedLimit(allowed[col]);
            output->setMaximumLimit(maximum[col]);
        }
    }
}

void NumberGrid::addValue(const QString& name, double value, const Quantity& quantity, int decimals) {
    addValues(name, {value}, {&quantity}, {}, {}, decimals);
}
