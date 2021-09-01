#pragma once
#include <QtWidgets>
#include "solver/numerics/Eigen.hpp"
#include "gui/units/UnitGroup.hpp"
#include "TableView.hpp"
#include "TableModel.hpp"
#include "solver/model/ProfileCurve.hpp"

class ProfileTreeHeader: public QHeaderView {
public:
    ProfileTreeHeader(QWidget* parent, const QList<QToolButton*>& buttons);
};

class ProfileTreeItem: public QTreeWidgetItem {
public:
    ProfileTreeItem(const SegmentInput& segment);
    QWidget* getEditor();

private:
    SegmentInput segment;
    QWidget* editor;

    QIcon segmentIcon(SegmentType type) const;
    QString segmentText(SegmentType type) const;
    QWidget* segmentEditor(SegmentType type) const;
};

class ProfileEditor: public QWidget
{
    Q_OBJECT

public:
    ProfileEditor();

    MatrixXd getData() const;
    void setData(const MatrixXd& data);

signals:
    void modified();
    void rowSelectionChanged(const QVector<int>& rows);

private:    
    QVector<int> getSelectedIndices();
};
