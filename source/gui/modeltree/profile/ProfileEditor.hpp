#pragma once
#include "solver/model/profile/ProfileInput.hpp"
#include <QWidget>

class ProfileTreeModel;
class ProfileTreeView;
class ProfileSegmentView;

class ProfileEditor: public QWidget {
    Q_OBJECT

public:
    ProfileEditor();

    ProfileInput getData() const;
    void setData(const ProfileInput& data);

signals:
    void modified();
    void selectionChanged(const QList<int>& indices);

private:
    ProfileTreeModel* profile_model;
    ProfileTreeView* profile_view;
    ProfileSegmentView* segment_view;

    QList<int> getSelectedIndices();
};
