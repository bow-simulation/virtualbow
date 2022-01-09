#pragma once
#include "solver/model/profile/ProfileInput.hpp"
#include <QStackedWidget>
#include <QHash>

class ProfileTreeModel;
class SegmentEditor;

class ProfileSegmentView: public QStackedWidget {
public:
    ProfileSegmentView(ProfileTreeModel* model);

    void showEditor(const QModelIndex& index);
    void hideEditor();

private:
    ProfileTreeModel* model;
    QHash<QPersistentModelIndex, QWidget*> editors;

    SegmentEditor* createSegmentEditor(const SegmentInput& input);
};
