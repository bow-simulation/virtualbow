#include "ProfileSegmentEditor.hpp"
#include "gui/widgets/DoubleSpinBox.hpp"

ProfileSegmentEditor::ProfileSegmentEditor(int rows, const QList<ConstraintType>& types, const QList<QString>& names, const QList<UnitGroup*>& units)
    : QTableWidget(rows, 2),
      types(types),
      units(units)
{
    this->setHorizontalHeaderLabels({"Property", "Value"});
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    this->verticalHeader()->setVisible(false);

    for(int i = 0; i < rows; ++i) {
        auto combo = new QComboBox();
        combo->setFrame(false);
        combo->addItems(names);

        auto spinner = new DoubleSpinBox(Domain::UNRESTRICTED);
        spinner->setFrame(false);

        combo_boxes.append(combo);
        spinners.append(spinner);

        this->setCellWidget(i, 0, combo);
        this->setCellWidget(i, 1, spinner);
    }

    /*
    combo1 = new QComboBox();
    combo1->setFrame(false);
    combo1->addItems({"Length", "Radius", "Angle"});

    combo2 = new QComboBox();
    combo2->setFrame(false);
    combo2->addItems({"Length", "Radius", "Angle"});

    spinner1 = new DoubleSpinBox(Domain::UNRESTRICTED);
    spinner1->setUnitGroup(&UnitSystem::length);
    spinner1->setFrame(false);

    spinner2 = new DoubleSpinBox(Domain::UNRESTRICTED);
    spinner2->setUnitGroup(&UnitSystem::length);
    spinner2->setFrame(false);

    this->setCellWidget(0, 0, combo1);
    this->setCellWidget(0, 1, spinner1);
    this->setCellWidget(1, 0, combo2);
    this->setCellWidget(1, 1, spinner2);
    */
}

SegmentInput ProfileSegmentEditor::getData() const {

}

void ProfileSegmentEditor::setData(const SegmentInput& data) {

}
