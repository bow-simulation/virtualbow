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

class DataViewModel;

class TreeItem: public QTreeWidgetItem
{
public:
    TreeItem(const QString& name, const QIcon& icon, TreeItemType type);

    QWidget* getEditor();
    QWidget* getPlot();

    virtual void insertChild(int i, QTreeWidgetItem* item);
    virtual void removeChild(int i);
    virtual void swapChildren(int i, int j);

    int row() const;

protected:
    QWidget* editor;
    QWidget* plot;

    void setEditor(QWidget* editor);
    void setPlot(QWidget* plot);
};
