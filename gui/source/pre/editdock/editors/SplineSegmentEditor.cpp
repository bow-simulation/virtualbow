#include "SplineSegmentEditor.hpp"
#include "pre/widgets/TableEditor.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"
#include <QVBoxLayout>

SplineSegmentEditor::SplineSegmentEditor()
    : table(new TableEditor("X", "Y", Quantities::length, Quantities::length, DoubleRange::unrestricted(1e-3), DoubleRange::unrestricted(1e-3)))
{
    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    vbox->setMargin(0);
    vbox->addWidget(table);

    QObject::connect(table, &TableEditor::modified, this, &SplineSegmentEditor::modified);
}

SegmentInput SplineSegmentEditor::getData() const {
    return table->getData();
}

void SplineSegmentEditor::setData(const SegmentInput& data) {
    if(auto value = std::get_if<SplineInput>(&data)) {
        table->setData(*value);
    }
    else {
        throw std::invalid_argument("Wrong segment type");
    }
}
