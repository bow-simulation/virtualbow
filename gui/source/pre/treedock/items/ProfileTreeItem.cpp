#include "ProfileTreeItem.hpp"
#include "pre/viewmodel/ViewModel.hpp"
#include "pre/widgets/propertytree/PropertyTreeWidget.hpp"
#include "pre/widgets/propertytree/items/GroupPropertyItem.hpp"
#include "pre/widgets/propertytree/items/DoublePropertyItem.hpp"
#include "pre/widgets/propertytree/items/IntegerPropertyItem.hpp"
#include "pre/editdock/editors/SegmentEditor.hpp"
#include "pre/editdock/editors/LineSegmentEditor.hpp"
#include "pre/editdock/editors/ArcSegmentEditor.hpp"
#include "pre/editdock/editors/SpiralSegmentEditor.hpp"
#include "pre/editdock/editors/SplineSegmentEditor.hpp"
#include "pre/plotdock/plots/ProfileView.hpp"

ProfileTreeItem::ProfileTreeItem(ViewModel* model)
    : TreeItem(model, "Profile", QIcon(":/icons/model-profile.svg"), 0)
{
    auto plot = new ProfileView(Quantities::length);
    setPlot(plot);
    QObject::connect(model, &ViewModel::profileModified, plot, [=]{
        plot->setData(model->getProfile());
    });

    QObject::connect(model, &ViewModel::reloaded, [=] {
        initFromModel();
    });

    QObject::connect(model, &ViewModel::segmentModified, [=](int i, void* source) {
        /*
        if(source != this) {
            auto item = dynamic_cast<SegmentTreeItem*>(this->child(i));
            item->setSegment(model->getProfile()[i]);
        }
        */
    });

    QObject::connect(model, &ViewModel::segmentInserted, [=](int i, void* source) {
        /*
        if(source != this) {
            auto item = new SegmentTreeItem(model, model->getProfile()[i]);
            this->insertChild(i, item);
        }
        */
    });

    QObject::connect(model, &ViewModel::segmentRemoved, [=](int i, void* source) {
        if(source != this) {
            this->removeChild(i);
        }
    });

    QObject::connect(model, &ViewModel::segmentsSwapped, [=](int i, int j, void* source) {
        if(source != this) {
            this->swapChildren(i, j);
        }
    });

    initFromModel();
}

void ProfileTreeItem::initFromModel() {
    /*
    this->removeChildren();
    for(auto& segment: model->getProfile()) {
        auto item = new SegmentTreeItem(model, segment);
        this->addChild(item);
    }
    */
}

/*
void ProfileTreeItem::updateModel(void* source) {
    Profile profile;
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
*/

SegmentTreeItem::SegmentTreeItem(ViewModel* model, const ProfileSegment& segment)
    : TreeItem(model, segmentName(segment), segmentIcon(segment), 0)
{
    SegmentEditor* editor = segmentEditor(segment);
    setEditor(editor);
    setSegment(segment);

    // Update viewmodel on changes
    QObject::connect(editor, &SegmentEditor::modified, [=]{
        model->modifySegment(row(), getSegment(), parent());
    });

}

ProfileSegment SegmentTreeItem::getSegment() const {
    return static_cast<SegmentEditor*>(editor)->getData();
}

void SegmentTreeItem::setSegment(const ProfileSegment& segment) {
    static_cast<SegmentEditor*>(editor)->setData(segment);
}

QString SegmentTreeItem::segmentName(const ProfileSegment& input) const {
    if(auto value = std::get_if<Line>(&input)) {
        return "Line";
    }
    if(auto value = std::get_if<Arc>(&input)) {
        return "Arc";
    }
    if(auto value = std::get_if<Spiral>(&input)) {
        return "Spiral";
    }
    if(auto value = std::get_if<Spline>(&input)) {
        return "Spline";
    }

    throw std::runtime_error("Unknown segment type");
}

QIcon SegmentTreeItem::segmentIcon(const ProfileSegment& input) const {
    if(auto value = std::get_if<Line>(&input)) {
        return QIcon(":/icons/segment-line.svg");
    }
    if(auto value = std::get_if<Arc>(&input)) {
        return QIcon(":/icons/segment-arc.svg");
    }
    if(auto value = std::get_if<Spiral>(&input)) {
        return QIcon(":/icons/segment-spiral.svg");
    }
    if(auto value = std::get_if<Spline>(&input)) {
        return QIcon(":/icons/segment-spline.svg");
    }

    throw std::runtime_error("Unknown segment type");
}

SegmentEditor* SegmentTreeItem::segmentEditor(const ProfileSegment& input) const {
    if(auto value = std::get_if<Line>(&input)) {
        return new LineSegmentEditor();
    }
    if(auto value = std::get_if<Arc>(&input)) {
        return new ArcSegmentEditor();
    }
    if(auto value = std::get_if<Spiral>(&input)) {
        return new SpiralSegmentEditor();
    }
    /*
    if(auto value = std::get_if<Spline>(&input)) {
        return new SplineSegmentEditor();
    }
    */

    throw std::runtime_error("Unknown segment type");
}
