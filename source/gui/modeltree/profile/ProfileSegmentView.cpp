#include "ProfileSegmentView.hpp"
#include "ProfileTreeModel.hpp"
#include "segments/LineSegmentEditor.hpp"
#include "segments/ArcSegmentEditor.hpp"
#include "segments/SpiralSegmentEditor.hpp"
#include "segments/SplineSegmentEditor.hpp"
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>

ProfileSegmentView::ProfileSegmentView(ProfileTreeModel* model)
    : model(model)
{
    // TODO: Grey text "no item selected" or "no segment selected"?
    // Or "No segments" and "No properties" top and bottom?
    auto placeholder = new QTableWidget();
    placeholder->horizontalHeader()->setVisible(false);
    placeholder->verticalHeader()->setVisible(false);

    addWidget(placeholder);
    setCurrentIndex(0);
}

void ProfileSegmentView::showEditor(const QModelIndex& index) {
    QPersistentModelIndex persistent(index);

    if(!editors.contains(persistent)) {
        SegmentInput segment = model->getSegment(persistent);

        auto editor = createSegmentEditor(segment);
        editor->setData(segment);

        // TODO: Exchange data between editor and model, keep editor up to date

        editors.insert(persistent, editor);
        addWidget(editor);
    }

    setCurrentWidget(editors[persistent]);
}

void ProfileSegmentView::hideEditor() {
    setCurrentIndex(0);
}

SegmentEditor* ProfileSegmentView::createSegmentEditor(const SegmentInput& input) {
    if(std::holds_alternative<LineInput>(input)) {
        return new LineSegmentEditor();
    }
    if(std::holds_alternative<ArcInput>(input)) {
        return new ArcSegmentEditor();
    }
    if(std::holds_alternative<SpiralInput>(input)) {
        return new SpiralSegmentEditor();
    }
    if(std::holds_alternative<SplineInput>(input)) {
        return new SplineSegmentEditor();
    }

    throw std::invalid_argument("Unknown segment type");
}
