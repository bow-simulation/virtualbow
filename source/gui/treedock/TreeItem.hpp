#pragma once
#include <QTreeWidgetItem>

enum TreeItemType
{
    COMMENTS = QTreeWidgetItem::UserType,  // Required by Qt
    SETTINGS,
    DIMENSIONS,
    MATERIALS,
    MATERIAL,
    LAYERS,
    LAYER,
    PROFILE,
    SEGMENT,
    WIDTH,
    STRING,
    MASSES,
    DAMPING
};

class ViewModel;

class TreeItem: public QTreeWidgetItem
{
public:
    TreeItem(ViewModel* model, const QString& name, const QIcon& icon, TreeItemType type);

    QWidget* getEditor();
    QWidget* getPlot();

    virtual void insertChild(int i, QTreeWidgetItem* item);
    virtual void swapChildren(int i, int j);
    virtual void removeChild(int i);
    virtual void removeChildren();

    int row() const;

protected:
    ViewModel* model;
    QWidget* editor;
    QWidget* plot;

    void setEditor(QWidget* editor);
    void setPlot(QWidget* plot);
};
