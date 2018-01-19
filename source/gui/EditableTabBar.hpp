#pragma once
#include <QtWidgets>

// TabBar with a plus button for adding tabs and the ability to rename tabs.
// Moving tabs enabled by default.

class EditableTabBar: public QTabWidget
{
    Q_OBJECT

public:
    EditableTabBar();

signals:
    void addTabRequested();
};

// Dialog used for renaming tabs.
// Todo: Use inline QLineEdit for renaming
class RenameDialog: public QDialog
{
public:
    RenameDialog(QWidget* parent);

    QString getText() const;
    void setText(QString text);

private:
    QLineEdit* editor;
};
