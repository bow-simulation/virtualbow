#pragma once
#include <QTabWidget>

// TabBar with a plus button for adding tabs and the ability to rename tabs.
// Moving tabs enabled by default.
// Todo: Find a way to use an inline QLineEdit for renaming tabs
class EditableTabBar: public QTabWidget
{
    Q_OBJECT

public:
    EditableTabBar();
    QString tabText(int index) const;

signals:
    void addTabRequested();
    void tabRenamed(int index);
};
