#include "OutputGrid.hpp"
#include "pre/widgets/DoubleOutput.hpp"
#include "pre/models/units/Quantity.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>

OutputGrid::OutputGrid()
    : columnLayout(new QHBoxLayout()),
      currentColumn(nullptr),
      currentGrid(nullptr)
{
    columnLayout->setSpacing(20);
    columnLayout->setContentsMargins(20, 20, 20, 20);
    columnLayout->addStretch();
    this->setLayout(columnLayout);
}

void OutputGrid::addColumn() {
    // Create new column
    currentColumn = new QVBoxLayout();
    currentColumn->addStretch();

    // Forget old grid
    currentGrid = nullptr;

    // Insert before horizontal stretch
    int i = columnLayout->count() - 1;
    columnLayout->insertLayout(i, currentColumn);
}

void OutputGrid::addGroup(const QString& name) {
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

void OutputGrid::addHeaders(const QStringList& headers) {
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

void OutputGrid::addValue(const QString& name, QWidget* widget) {
    addValues(name, {widget});
}

void OutputGrid::addValues(const QString& name, QList<QWidget*> widgets) {
    if(currentColumn == nullptr) {
        addColumn();
    }

    if(currentGrid == nullptr) {
        addGroup("Default");
    }

    int row = currentGrid->rowCount();
    auto label = new QLabel(name);

    currentGrid->addWidget(label, row, 0);
    for(int col = 0; col < widgets.size(); ++col) {
        currentGrid->addWidget(widgets[col], row, col + 1);
    }
}

