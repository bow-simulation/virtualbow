#include "PropertyValueEditor.hpp"
#include "pre/widgets/DoubleSpinBox.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"
#include "pre/utils/DoubleRange.hpp"
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QStackedWidget>

PropertyValueEditor::PropertyValueEditor(int rows, const QList<QString>& names,  const QList<Quantity*>& quantities, const QList<DoubleRange>& ranges) {
    auto table = new QTableWidget(rows, 2);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->horizontalHeader()->setVisible(false);
    table->verticalHeader()->setVisible(false);

    for(int i = 0; i < rows; ++i) {
        auto stack = new QStackedWidget();
        auto combo = new QComboBox();
        combo->setFrame(false);

        for(int j = 0; j < names.size(); ++j) {
            auto spinner = new DoubleSpinBox(*quantities[i], ranges[i]);
            QObject::connect(spinner, &DoubleSpinBox::modified, this, &PropertyValueEditor::modified);
            spinner->setFrame(false);

            combo->addItem(names[j], j);
            stack->addWidget(spinner);
        }

        table->setCellWidget(i, 0, combo);
        table->setCellWidget(i, 1, stack);

        combos.push_back(combo);
        stacks.push_back(stack);

        //int last_index = combo->currentIndex();
        QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int current_index) mutable {
            /*
            // Check if the selected index is already selected in another combobox
            // If so, set the other combobox to the previous value of this one (i.e. switch the two)
            for(QComboBox* other: combos) {
                if(other != combo && other->currentIndex() == current_index) {
                    other->setCurrentIndex(last_index);
                }
            }
            */

            // Show the associated widget for the selected index
            stack->setCurrentIndex(current_index);

            // Remember index and send modification signal
            //last_index = current_index;
            emit modified();
        });
    }

    auto vbox = new QVBoxLayout();
    vbox->setMargin(0);
    vbox->addWidget(table);

    this->setLayout(vbox);
}
