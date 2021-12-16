#pragma once
#include "solver/model/profile/ProfileInput.hpp"
#include <QWidget>

class ProfileTree;

class ProfileEditor: public QWidget {
    Q_OBJECT

public:
    ProfileEditor();

    ProfileInput getData() const;
    void setData(const ProfileInput& data);

signals:
    void modified();
    void rowSelectionChanged(const QVector<int>& rows);

private:
    ProfileTree* profile_tree;

    QVector<int> getSelectedIndices();
};
