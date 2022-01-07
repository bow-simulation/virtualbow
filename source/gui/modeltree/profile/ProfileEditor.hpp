#pragma once
#include "solver/model/profile/ProfileInput.hpp"
#include <QWidget>

class ProfileTreeView;

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
    ProfileTreeView* profile_tree;

    QList<int> getSelectedIndices();
};
