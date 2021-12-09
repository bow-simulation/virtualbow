#include "ProfileSegmentEditor.hpp"
#include "gui/widgets/DoubleSpinBox.hpp"

ProfileSegmentEditor::ProfileSegmentEditor()
/*
      constraint_types({ LineConstraint::LENGTH }),
      constraint_names({ "Length" }),
      constraint_units({ &UnitSystem::length })
*/
{
    /*
    this->setHorizontalHeaderLabels({"Property", "Value"});
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    this->verticalHeader()->setVisible(false);

    for(int i = 0; i < rowCount(); ++i) {
        auto combo = new QComboBox();
        combo->setFrame(false);
        for(int j = 0; j < constraint_names.size(); ++j) {
            combo->addItem(constraint_names[j], static_cast<int>(constraint_types[j]));
        }

        auto spinner = new DoubleSpinBox(Domain::UNRESTRICTED);
        spinner->setFrame(false);

        auto update_spinner_unit = [this, spinner](int index){
            QSignalBlocker blocker(spinner);    // Block modification signal
            spinner->setValue(0.0);
            spinner->setUnitGroup(constraint_units[index]);
        };

        QObject::connect(spinner, QOverload<double>::of(&DoubleSpinBox::valueChanged), this, &LineSegmentEditor::modified);
        QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LineSegmentEditor::modified);
        QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, update_spinner_unit);
        update_spinner_unit(combo->currentIndex());

        this->setCellWidget(i, 0, combo);
        this->setCellWidget(i, 1, spinner);
    }
    */
}

/*
SegmentInput ProfileSegmentEditor::getData() const {
    SegmentInput data = {.type = segment_type };
    for(int i = 0; i < this->rowCount(); ++i) {
        auto combo = dynamic_cast<QComboBox*>(this->cellWidget(i, 0));
        auto spinner = dynamic_cast<DoubleSpinBox*>(this->cellWidget(i, 1));

        data.constraints.push_back(Constraint{
            .type = static_cast<ConstraintType>(combo->currentData().toInt()),
            .value = spinner->value()
        });
    }

    return data;
}
*/

/*
void ProfileSegmentEditor::setData(const SegmentInput& data) {
    QSignalBlocker blocker(this);    // Block modification signals

    if(data.constraints.size() != this->rowCount()) {
        throw std::invalid_argument("Invalid number of constraints for widget");
    }

    for(int i = 0; i < this->rowCount(); ++i) {
        auto combo = dynamic_cast<QComboBox*>(this->cellWidget(i, 0));
        auto spinner = dynamic_cast<DoubleSpinBox*>(this->cellWidget(i, 1));

        for(int j = 0; j < combo->count(); ++j) {
            if(combo->itemData(j) == static_cast<int>(data.constraints[i].type)) {
                combo->setCurrentIndex(j);
                break;
            }
        }

        spinner->setValue(data.constraints[i].value);
    }
}
*/

/*
int ProfileSegmentEditor::numConstraints(SegmentType segment_type) {
    switch(segment_type) {
        case SegmentType::Line: return 1;
        case SegmentType::Arc: return 2;
        case SegmentType::Spiral: return 3;
        default: throw std::invalid_argument("Invalid segment type");
    }
}

QList<ConstraintType> ProfileSegmentEditor::constraintTypes(SegmentType segment_type) {
    switch(segment_type) {
        case SegmentType::Line: return {ConstraintType::X_END, ConstraintType::Y_END, ConstraintType::S_END, ConstraintType::DELTA_X, ConstraintType::DELTA_Y, ConstraintType::DELTA_S};
        case SegmentType::Arc: return {ConstraintType::X_END, ConstraintType::Y_END, ConstraintType::S_END, ConstraintType::PHI_END, ConstraintType::DELTA_X, ConstraintType::DELTA_Y, ConstraintType::DELTA_PHI, ConstraintType::R_START};
        case SegmentType::Spiral: return {ConstraintType::X_END, ConstraintType::Y_END, ConstraintType::S_END, ConstraintType::PHI_END, ConstraintType::DELTA_X, ConstraintType::DELTA_Y, ConstraintType::DELTA_PHI, ConstraintType::R_START, ConstraintType::R_END};
        default: throw std::invalid_argument("Invalid segment type");
    }
}

QList<QString> ProfileSegmentEditor::constraintNames(SegmentType segment_type) {
    switch(segment_type) {
        case SegmentType::Line: return {"X End", "Y End", "S End", "Delta X", "Delta Y", "Delta S"};
        case SegmentType::Arc: return {"X End", "Y End", "Phi End", "Delta X", "Delta Y", "Delta Phi", "R Start"};
        case SegmentType::Spiral: return {"X End", "Y End", "Phi End", "Delta X", "Delta Y", "Delta Phi", "R Start", "R End"};
        default: throw std::invalid_argument("Invalid segment type");
    }
}

QList<UnitGroup*> ProfileSegmentEditor::constraintUnits(SegmentType segment_type) {
    switch(segment_type) {
        case SegmentType::Line: return {&UnitSystem::length, &UnitSystem::length, &UnitSystem::length, &UnitSystem::length, &UnitSystem::length, &UnitSystem::length};
        case SegmentType::Arc: return {&UnitSystem::length, &UnitSystem::length, &UnitSystem::angle, &UnitSystem::length, &UnitSystem::length, &UnitSystem::angle, &UnitSystem::length};
        case SegmentType::Spiral: return {&UnitSystem::length, &UnitSystem::length, &UnitSystem::angle, &UnitSystem::length, &UnitSystem::length, &UnitSystem::angle, &UnitSystem::length, &UnitSystem::length};
        default: throw std::invalid_argument("Invalid segment type");
    }
}
*/
