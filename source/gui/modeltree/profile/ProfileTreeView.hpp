#pragma once
#include "solver/model/profile/ProfileInput.hpp"
#include <QTreeView>
#include <QHeaderView>

class ProfileTreeView;
class ProfileTreeModel;
class SegmentEditor;
class QToolButton;
class QWidget;
class QTableWidget;

class ProfileTreeHeader: public QHeaderView
{
public:
    ProfileTreeHeader(QWidget* parent, const QList<QToolButton*>& buttons);
};

class ProfileTreeView: public QTreeView
{
    Q_OBJECT

public:
    ProfileTreeView();

    ProfileInput getData() const;
    void setData(const ProfileInput& data);

signals:
    void modified();

private:
    ProfileTreeModel* model;
    QToolButton* button_add;
    QToolButton* button_remove;
    QToolButton* button_up;
    QToolButton* button_down;

    void onButtonAdd(const SegmentInput& input);
    void updateButtons();
};
