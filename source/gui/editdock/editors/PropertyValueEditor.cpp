#include "PropertyValueEditor.hpp"
#include "gui/widgets/DoubleSpinBox.hpp"
#include "gui/viewmodel/units/UnitSystem.hpp"
#include "gui/utils/DoubleRange.hpp"
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QStackedWidget>

PropertyValueEditor::PropertyValueEditor(int rows, const QList<QString>& names,  const QList<UnitGroup*>& units, const QList<DoubleRange>& ranges) {
    auto table = new QTableWidget(rows, 2);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->horizontalHeader()->setVisible(false);
    table->verticalHeader()->setVisible(false);

    for(int i = 0; i < rows; ++i) {
        auto stack = new QStackedWidget();
        auto combo = new QComboBox();
        combo->setFrame(false);

        for(int j = 0; j < names.size(); ++j) {
            auto spinner = new DoubleSpinBox(*units[i], ranges[i]);
            QObject::connect(spinner, QOverload<double>::of(&DoubleSpinBox::valueChanged), this, &PropertyValueEditor::modified);
            spinner->setFrame(false);

            combo->addItem(names[j], j);
            stack->addWidget(spinner);
        }

        table->setCellWidget(i, 0, combo);
        table->setCellWidget(i, 1, stack);

        combos.push_back(combo);
        stacks.push_back(stack);

        QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PropertyValueEditor::modified);
        QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [stack](int index){
            stack->setCurrentIndex(index);
        });

        /*
        // Update the unis of the spinner depending on the combobox selection
        auto update_spinner_unit = [this, spinner](int index) {
            QSignalBlocker blocker(spinner);    // Block modification signal
            spinner->setValue(0.0);
            //spinner->setUnitGroup(this->units[index]);
        };

        QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PropertyValueEditor::modified);
        QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, update_spinner_unit);
        //QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, update_combos_enabled);
        update_spinner_unit(combo->currentIndex());
        */
    }

    auto vbox = new QVBoxLayout();
    vbox->setMargin(0);
    vbox->addWidget(table);

    this->setLayout(vbox);
}
