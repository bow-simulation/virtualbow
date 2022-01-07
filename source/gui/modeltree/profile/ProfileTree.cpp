#include "ProfileTree.hpp"
#include "segments/LineSegmentEditor.hpp"
#include "segments/ArcSegmentEditor.hpp"
#include "segments/SpiralSegmentEditor.hpp"
#include "segments/SplineSegmentEditor.hpp"
#include <QHBoxLayout>
#include <QToolButton>
#include <QPushButton>
#include <QAction>
#include <QMenu>

ProfileTreeItem::ProfileTreeItem(ProfileTree* parent, const SegmentInput& input)
    : editor(segmentEditor(input))
{
    this->setFlags(flags() & ~Qt::ItemIsDropEnabled);
    this->setText(0, segmentText(input));
    this->setIcon(0, segmentIcon(input));

    editor->setData(input);

    QObject::connect(editor, &SegmentEditor::modified, parent, &ProfileTree::modified);
    parent->addTopLevelItem(this);
}

SegmentEditor* ProfileTreeItem::getEditor() {
    return editor;
}

SegmentInput ProfileTreeItem::getData() const {
    return editor->getData();
}

QIcon ProfileTreeItem::segmentIcon(const SegmentInput& input) const {
    if(std::holds_alternative<LineInput>(input)) {
        return QIcon(":/icons/segment-line.svg");
    }
    if(std::holds_alternative<ArcInput>(input)) {
        return QIcon(":/icons/segment-arc.svg");
    }
    if(std::holds_alternative<SpiralInput>(input)) {
        return QIcon(":/icons/segment-spiral.svg");
    }
    if(std::holds_alternative<SplineInput>(input)) {
        return QIcon(":/icons/segment-spline.svg");
    }

    throw std::invalid_argument("Unknown segment type");
}

QString ProfileTreeItem::segmentText(const SegmentInput& input) const {
    if(std::holds_alternative<LineInput>(input)) {
        return "Line";
    }
    if(std::holds_alternative<ArcInput>(input)) {
        return "Arc";
    }
    if(std::holds_alternative<SpiralInput>(input)) {
        return "Spiral";
    }
    if(std::holds_alternative<SplineInput>(input)) {
        return "Spline";
    }

    throw std::invalid_argument("Unknown segment type");
}

SegmentEditor* ProfileTreeItem::segmentEditor(const SegmentInput& input) const {
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

ProfileTreeHeader::ProfileTreeHeader(QWidget* parent, const QList<QToolButton*>& buttons)
    : QHeaderView(Qt::Horizontal, parent)
{
    auto hbox = new QHBoxLayout();
    hbox->setMargin(2);
    hbox->setSpacing(2);
    hbox->addStretch();
    for(auto button: buttons) {
        hbox->addWidget(button);
    }

    this->setLayout(hbox);
};

ProfileTree::ProfileTree() {
    auto button_add = new QToolButton();
    button_add->setIcon(QIcon(":/icons/list-add.svg"));
    button_add->setPopupMode(QToolButton::InstantPopup);
    button_add->setMenu(new QMenu());

    auto button_remove = new QToolButton();
    button_remove->setIcon(QIcon(":/icons/list-remove.svg"));

    auto action_remove = new QAction(this);
    action_remove->setShortcut(QKeySequence::Delete);
    this->addAction(action_remove);

    // Actions for adding and removing segments

    button_add->menu()->addAction(QIcon(":/icons/segment-line.svg"), "Line", this, [&] {
        new ProfileTreeItem(this, LineInput());
        emit modified();
    });

    button_add->menu()->addAction(QIcon(":/icons/segment-arc.svg"), "Arc", this, [&] {
        new ProfileTreeItem(this, ArcInput());
        emit modified();
    });

    button_add->menu()->addAction(QIcon(":/icons/segment-spiral.svg"), "Spiral", this, [&] {
        new ProfileTreeItem(this, SpiralInput());
        emit modified();
    });

    button_add->menu()->addAction(QIcon(":/icons/segment-spline.svg"), "Spline", this, [&] {
        new ProfileTreeItem(this, SplineInput());
        emit modified();
    });

    QObject::connect(button_remove, &QPushButton::clicked, this, [&] {
        int count = this->topLevelItemCount();
        if(count != 0) {
            QTreeWidgetItem* item = this->topLevelItem(count - 1);
            delete item;    // https://stackoverflow.com/a/9399167
            emit modified();
        }
    });

    QObject::connect(action_remove, &QAction::triggered, this, [&] {
        for(auto item: this->selectedItems()) {
            delete item;    // https://stackoverflow.com/a/9399167
        }
        emit modified();
    });

    // TODO
    // auto header = ...
    // ...
    //
    // this->setHeader(header);

    this->setHeader(new ProfileTreeHeader(this, {button_add, button_remove}));
    this->header()->setStretchLastSection(false);
    this->header()->setSectionResizeMode(QHeaderView::Stretch);
    this->header()->setDefaultAlignment(Qt::AlignLeft);
    this->setHeaderLabel("Segments");
    this->setRootIsDecorated(false);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setDragDropMode(QAbstractItemView::InternalMove);
    this->setDragEnabled(true);
    this->setAcceptDrops(true);
    this->setDropIndicatorShown(true);
}

void ProfileTree::dropEvent(QDropEvent *event) {
    QTreeWidget::dropEvent(event);
    emit modified();
}

ProfileInput ProfileTree::getData() const {
    ProfileInput result;
    for(int i = 0; i < this->topLevelItemCount(); ++i) {
        auto item = dynamic_cast<ProfileTreeItem*>(this->topLevelItem(i));
        result.push_back(item->getData());
    }

    return result;
}

void ProfileTree::setData(const ProfileInput& data) {
    this->clear();
    for(auto& segment: data) {
        auto item = new ProfileTreeItem(this, segment);
    }
}