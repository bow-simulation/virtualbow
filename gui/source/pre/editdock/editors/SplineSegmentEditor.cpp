#include "SplineSegmentEditor.hpp"
#include "pre/widgets/TableEditor.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"
#include <QVBoxLayout>

SplineSegmentEditor::SplineSegmentEditor()
    : table(new TableEditor("X", "Y", Quantities::length, Quantities::length, DoubleRange::unrestricted(1e-3), DoubleRange::unrestricted(1e-3)))
{
    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->addWidget(table);

    QObject::connect(table, &TableEditor::modified, this, &SplineSegmentEditor::modified);
}

ProfileSegment SplineSegmentEditor::getData() const {
    throw std::invalid_argument("Removed code");
    //return table->getData();
}

void SplineSegmentEditor::setData(const ProfileSegment& data) {
    throw std::invalid_argument("Removed code");
    /*
    if(auto value = std::get_if<SpLine>(&data)) {
        table->setData(*value);
    }
    else {
        throw std::invalid_argument("Wrong segment type");
    }
    */
}
