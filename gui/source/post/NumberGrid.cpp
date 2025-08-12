#include "NumberGrid.hpp"
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

void NumberGrid::addValues(const QString& name, const Quantity& quantity, const QList<double>& values) {
    if(currentColumn == nullptr) {
        addColumn();
    }

    if(currentGrid == nullptr) {
        addGroup("Default");
    }

    int row = currentGrid->rowCount();

    auto label = new QLabel();
    currentGrid->addWidget(label, row, 0, Qt::AlignRight);

    for(int col = 0; col < values.size(); ++col) {
        auto edit = new QLineEdit();
        edit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        edit->setAlignment(Qt::AlignCenter);
        edit->setReadOnly(true);

        currentGrid->addWidget(edit, row, col + 1);

        double value = values[col];
        auto update = [&, name, value, label, edit] {
            label->setText(name + " " + quantity.getUnit().getSuffix());
            edit->setText(QString::number(quantity.getUnit().fromBase(value)));
        };

        QObject::connect(&quantity, &Quantity::unitChanged, this, update);
        update();
    }
}

void NumberGrid::addValue(const QString& name, const Quantity& quantity, double value) {
    addValues(name, quantity, {value});
}
