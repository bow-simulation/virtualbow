#include "PropertyValueEditor.hpp"
#include "gui/widgets/DoubleSpinBox.hpp"
#include "gui/units/UnitSystem.hpp"
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>

PropertyValueEditor::PropertyValueEditor(int rows, const QList<QString>& names,  const QList<UnitGroup*>& units)
    : units(units)
{
    auto table = new QTableWidget(rows, 2);
    table->setHorizontalHeaderLabels({"Property", "Value"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    table->verticalHeader()->setVisible(false);

    for(int i = 0; i < rows; ++i) {
        auto combo = new QComboBox();
        combo->setFrame(false);
        for(int j = 0; j < names.size(); ++j) {
            combo->addItem(names[j], j);
        }

        auto spinner = new DoubleSpinBox(Domain::UNRESTRICTED);
        spinner->setFrame(false);

        // Update the unis of the spinner depending on the combobox selection
        auto update_spinner_unit = [this, spinner](int index) {
            QSignalBlocker blocker(spinner);    // Block modification signal
            spinner->setValue(0.0);
            spinner->setUnitGroup(this->units[index]);
        };

        /*
        // Set the selected entry to disabled in all other comboboxes
        auto update_combos_enabled = [this, combo](int index) {
            for(auto c: combos) {
                // http://stackoverflow.com/q/38915001
                auto model = qobject_cast<QStandardItemModel*>(c->model());
                for(int i = 0; i < model->rowCount(); ++i) {
                    QStandardItem* item = model->item(i);
                    if(i == index && c != combo) {
                        item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
                    }
                    else if(c != combo) {
                        item->setFlags(item->flags() | Qt::ItemIsEnabled);
                    }
                }
            }
        };
        */

        QObject::connect(spinner, QOverload<double>::of(&DoubleSpinBox::valueChanged), this, &PropertyValueEditor::modified);
        QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PropertyValueEditor::modified);
        QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, update_spinner_unit);
        //QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, update_combos_enabled);
        update_spinner_unit(combo->currentIndex());

        combos.push_back(combo);
        spinners.push_back(spinner);

        table->setCellWidget(i, 0, combo);
        table->setCellWidget(i, 1, spinner);
    }

    auto vbox = new QVBoxLayout();
    vbox->setMargin(0);
    vbox->addWidget(table);

    this->setLayout(vbox);
}
