#include "SplineSegmentEditor.hpp"
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>

SplineSegmentEditor::SplineSegmentEditor() {
    auto table = new QTableWidget(50, 2);
    table->setHorizontalHeaderLabels({"X [mm]", "Y [mm]"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    table->verticalHeader()->setVisible(false);

    auto vbox = new QVBoxLayout();
    vbox->setMargin(0);
    vbox->addWidget(table);

    this->setLayout(vbox);
}

SegmentInput SplineSegmentEditor::getData() const {
    return SplineInput();
}

void SplineSegmentEditor::setData(const SegmentInput& data) {

}
