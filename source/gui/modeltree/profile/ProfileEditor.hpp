#pragma once
#include "ProfileTree.hpp"
#include "solver/model/profile/ProfileCurve.hpp"
#include <QtWidgets>

class ProfileEditor: public QWidget {
    Q_OBJECT

public:
    ProfileEditor();

    std::vector<SegmentInput> getData() const;
    void setData(const std::vector<SegmentInput>& data);

signals:
    void modified();
    void rowSelectionChanged(const QVector<int>& rows);

private:
    ProfileTree* profile_tree;

    QVector<int> getSelectedIndices();
};
