#include "ProfileTreeItem.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include "gui/widgets/propertytree/PropertyTreeWidget.hpp"
#include "gui/widgets/propertytree/items/GroupPropertyItem.hpp"
#include "gui/widgets/propertytree/items/DoublePropertyItem.hpp"
#include "gui/widgets/propertytree/items/IntegerPropertyItem.hpp"
#include "gui/modeltree/profile/SegmentEditor.hpp"
#include "gui/modeltree/profile/segments/LineSegmentEditor.hpp"
#include "gui/modeltree/profile/segments/ArcSegmentEditor.hpp"
#include "gui/modeltree/profile/segments/SpiralSegmentEditor.hpp"
#include "gui/modeltree/profile/segments/SplineSegmentEditor.hpp"

ProfileTreeItem::ProfileTreeItem(DataViewModel* model)
    : TreeItem("Profile", QIcon(":/icons/model-profile.svg"), TreeItemType::PROFILE),
      model(model)
{
    updateView();
}

void ProfileTreeItem::updateModel() {
    ProfileInput profile;
    for(int i = 0; i < this->childCount(); ++i) {
        auto item = dynamic_cast<SegmentTreeItem*>(this->child(i));
        if(item != nullptr) {
            profile.push_back(item->getSegment());
        }
    }
    model->setProfile(profile);
}

void ProfileTreeItem::updateView() {
    for(auto& segment: model->getProfile()) {
        auto item = new SegmentTreeItem(segment);
        this->addChild(item);
    }
}

void ProfileTreeItem::insertChild(int i, QTreeWidgetItem* item) {
    TreeItem::insertChild(i, item);
    updateModel();
}

void ProfileTreeItem::removeChild(int i) {
    TreeItem::removeChild(i);
    updateModel();
}

void ProfileTreeItem::swapChildren(int i, int j) {
    TreeItem::swapChildren(i, j);
    updateModel();
}

SegmentTreeItem::SegmentTreeItem(const SegmentInput& input)
    : TreeItem(segmentName(input), segmentIcon(input), TreeItemType::SEGMENT)
{
    SegmentEditor* editor = segmentEditor(input);
    setEditor(editor);

    editor->setData(input);
    QObject::connect(editor, &SegmentEditor::modified, [&]{
        auto parent = dynamic_cast<ProfileTreeItem*>(this->parent());
        if(parent != nullptr) {
            parent->updateModel();
        }
    });
}

SegmentInput SegmentTreeItem::getSegment() const {
    return SegmentInput();
}

/*
QVariant SegmentTreeItem::data(int column, int role) const {
    if(role == Qt::DisplayRole) {
        return "(" + QString::number(this->row() + 1) + ") " + QTreeWidgetItem::data(column, role).toString();
    }

    return QTreeWidgetItem::data(column, role);
}
*/

QString SegmentTreeItem::segmentName(const SegmentInput& input) const {
    if(auto value = std::get_if<LineInput>(&input)) {
        return "Line";
    }
    if(auto value = std::get_if<ArcInput>(&input)) {
        return "Arc";
    }
    if(auto value = std::get_if<SpiralInput>(&input)) {
        return "Spiral";
    }
    if(auto value = std::get_if<SplineInput>(&input)) {
        return "Spline";
    }

    throw std::runtime_error("Unknown segment type");
}

QIcon SegmentTreeItem::segmentIcon(const SegmentInput& input) const {
    if(auto value = std::get_if<LineInput>(&input)) {
        return QIcon(":/icons/segment-line.svg");
    }
    if(auto value = std::get_if<ArcInput>(&input)) {
        return QIcon(":/icons/segment-arc.svg");
    }
    if(auto value = std::get_if<SpiralInput>(&input)) {
        return QIcon(":/icons/segment-spiral.svg");
    }
    if(auto value = std::get_if<SplineInput>(&input)) {
        return QIcon(":/icons/segment-spline.svg");
    }

    throw std::runtime_error("Unknown segment type");
}

SegmentEditor* SegmentTreeItem::segmentEditor(const SegmentInput& input) const {
    if(auto value = std::get_if<LineInput>(&input)) {
        return new LineSegmentEditor();
    }
    if(auto value = std::get_if<ArcInput>(&input)) {
        return new ArcSegmentEditor();
    }
    if(auto value = std::get_if<SpiralInput>(&input)) {
        return new SpiralSegmentEditor();
    }
    if(auto value = std::get_if<SplineInput>(&input)) {
        return new SplineSegmentEditor();
    }

    throw std::runtime_error("Unknown segment type");
}
