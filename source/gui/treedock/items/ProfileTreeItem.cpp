#include "ProfileTreeItem.hpp"
#include "gui/viewmodel/ViewModel.hpp"
#include "gui/widgets/propertytree/PropertyTreeWidget.hpp"
#include "gui/widgets/propertytree/items/GroupPropertyItem.hpp"
#include "gui/widgets/propertytree/items/DoublePropertyItem.hpp"
#include "gui/widgets/propertytree/items/IntegerPropertyItem.hpp"
#include "gui/editdock/editors/SegmentEditor.hpp"
#include "gui/editdock/editors/LineSegmentEditor.hpp"
#include "gui/editdock/editors/ArcSegmentEditor.hpp"
#include "gui/editdock/editors/SpiralSegmentEditor.hpp"
#include "gui/editdock/editors/SplineSegmentEditor.hpp"
#include "gui/plotdock/plots/ProfileView.hpp"

ProfileTreeItem::ProfileTreeItem(ViewModel* model)
    : TreeItem(model, "Profile", QIcon(":/icons/model-profile.svg"), TreeItemType::PROFILE)
{
    setPlot(new ProfileView(Quantities::length));

    updateView(nullptr);
    QObject::connect(model, &ViewModel::profileModified, [=](void* source){
        updateView(source);
    });
}

void ProfileTreeItem::updateModel(void* source) {
    ProfileInput profile;
    for(int i = 0; i < this->childCount(); ++i) {
        auto item = dynamic_cast<SegmentTreeItem*>(this->child(i));
        if(item != nullptr) {
            profile.push_back(item->getSegment());
        }
    }

    model->setProfile(profile, source);
}

void ProfileTreeItem::updateView(void* source) {
    if(source != this) {
        // Remove children without notifying the model
        while(this->childCount() > 0) {
            QTreeWidgetItem::removeChild(this->child(0));
        }

        // Add new children
        for(auto& segment: model->getProfile()) {
            auto item = new SegmentTreeItem(model, segment);
            this->addChild(item);
        }
    }

    // Update plot
    auto profile_view = static_cast<ProfileView*>(plot);
    profile_view->setData(model->getProfile());
}

SegmentTreeItem::SegmentTreeItem(ViewModel* model, const SegmentInput& input)
    : TreeItem(model, segmentName(input), segmentIcon(input), TreeItemType::SEGMENT)
{
    SegmentEditor* editor = segmentEditor(input);
    setEditor(editor);

    editor->setData(input);
    QObject::connect(editor, &SegmentEditor::modified, [=]{
        updateModel(parent());
    });
}

SegmentInput SegmentTreeItem::getSegment() const {
    return static_cast<SegmentEditor*>(editor)->getData();
}

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
