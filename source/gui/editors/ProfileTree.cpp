#include "ProfileTree.hpp"

ProfileTreeItem::ProfileTreeItem(ProfileTree* parent, SegmentType type)
    : editor(segmentEditor(type))
{
    this->setText(0, segmentText(type));
    this->setIcon(0, segmentIcon(type));
    this->setFlags(flags() & ~Qt::ItemIsDropEnabled);

    QObject::connect(editor, &ProfileSegmentEditor::modified, parent, &ProfileTree::modified);
    parent->addTopLevelItem(this);
}

ProfileSegmentEditor* ProfileTreeItem::getEditor() {
    return editor;
}

SegmentInput ProfileTreeItem::getData() const {
    return editor->getData();
}

void ProfileTreeItem::setData(const SegmentInput& data) {
    editor->setData(data);
}

QIcon ProfileTreeItem::segmentIcon(SegmentType type) const {
    switch(type) {
        case SegmentType::Line: return QIcon(":/icons/segment-line.svg");
        case SegmentType::Arc: return QIcon(":/icons/segment-arc.svg");
        case SegmentType::Spiral: return QIcon(":/icons/segment-spiral.svg");
        case SegmentType::Spline: return QIcon(":/icons/segment-spline.svg");
        default: throw std::invalid_argument("Unknown segment type");
    }
}

QString ProfileTreeItem::segmentText(SegmentType type) const {
    switch(type) {
        case SegmentType::Line: return "Line";
        case SegmentType::Arc: return "Arc";
        case SegmentType::Spiral: return "Spiral";
        case SegmentType::Spline: return "Spline";
        default: throw std::invalid_argument("Unknown segment type");
    }
}

ProfileSegmentEditor* ProfileTreeItem::segmentEditor(SegmentType type) const {
    switch(type) {
        case SegmentType::Line:
        case SegmentType::Arc:
        case SegmentType::Spiral:
            return new ProfileSegmentEditor(type);

        case SegmentType::Spline: throw std::invalid_argument("Not implemented");
        default: throw std::invalid_argument("Unknown segment type");
    }
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
        new ProfileTreeItem(this, SegmentType::Line);
        emit modified();
    });

    button_add->menu()->addAction(QIcon(":/icons/segment-arc.svg"), "Arc", this, [&] {
        new ProfileTreeItem(this, SegmentType::Arc);
        emit modified();
    });

    button_add->menu()->addAction(QIcon(":/icons/segment-spiral.svg"), "Spiral", this, [&] {
        new ProfileTreeItem(this, SegmentType::Spiral);
        emit modified();
    });

    button_add->menu()->addAction(QIcon(":/icons/segment-spline.svg"), "Spline", this, [&] {
        new ProfileTreeItem(this, SegmentType::Spline);
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

std::vector<SegmentInput> ProfileTree::getData() const {
    std::vector<SegmentInput> result;
    for(int i = 0; i < this->topLevelItemCount(); ++i) {
        auto item = dynamic_cast<ProfileTreeItem*>(this->topLevelItem(i));
        result.push_back(item->getData());
    }

    return result;
}

void ProfileTree::setData(const std::vector<SegmentInput>& data) {
    this->clear();
    for(auto& segment: data) {
        auto item = new ProfileTreeItem(this, segment.type);
        item->setData(segment);
    }
}
