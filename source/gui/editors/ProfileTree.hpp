#pragma once
#include "ProfileSegmentEditor.hpp"
#include "solver/model/ProfileCurve.hpp"
#include <QtWidgets>

class ProfileTree;

class ProfileTreeItem: public QTreeWidgetItem {
public:
    ProfileTreeItem(ProfileTree* parent, SegmentType type);
    ProfileSegmentEditor* getEditor();

    SegmentInput getData() const;
    void setData(const SegmentInput& data);

private:
    ProfileSegmentEditor* editor;

    QIcon segmentIcon(SegmentType type) const;
    QString segmentText(SegmentType type) const;
    ProfileSegmentEditor* segmentEditor(SegmentType type) const;
};

class ProfileTreeHeader: public QHeaderView {
public:
    ProfileTreeHeader(QWidget* parent, const QList<QToolButton*>& buttons);
};

class ProfileTree: public QTreeWidget {
    Q_OBJECT

public:
    ProfileTree();

    std::vector<SegmentInput> getData() const;
    void setData(const std::vector<SegmentInput>& data);

protected:
    void dropEvent(QDropEvent *event) override;

signals:
    void modified();
};
