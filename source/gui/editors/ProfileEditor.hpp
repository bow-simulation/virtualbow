#pragma once
#include <QtWidgets>
#include "solver/numerics/Eigen.hpp"
#include "gui/units/UnitGroup.hpp"
#include "TableView.hpp"
#include "TableModel.hpp"
#include "solver/model/ProfileCurve.hpp"

class SpinBoxDelegate: public QStyledItemDelegate {
    Q_OBJECT

public:
    SpinBoxDelegate(QObject* parent = nullptr);

    QWidget *createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

class ComboBoxDelegate: public QStyledItemDelegate {
    Q_OBJECT

public:
    ComboBoxDelegate(QObject* parent = nullptr);

    QWidget *createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};


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
    ProfileEditor(const UnitSystem& units);

    MatrixXd getData() const;
    void setData(const MatrixXd& data);

signals:
    void modified();
    void rowSelectionChanged(const QVector<int>& rows);

private:    
    QVector<int> getSelectedIndices();
};
