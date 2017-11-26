#pragma once
#include <QtWidgets>

// TabBar with a plus button for adding tabs and the ability to rename tabs. Moving tabs enabled by default.
// Special thanks to stackoverflow.com:
// Adding Tabs: https://stackoverflow.com/questions/19975137/how-can-i-add-a-new-tab-button-next-to-the-tabs-of-a-qmdiarea-in-tabbed-view-m
// Todo: Inline editing of tab names: https://stackoverflow.com/questions/8707457/pyqt-editable-tab-labels
class EditableTabBar: public QTabBar
{
    Q_OBJECT

public:
    EditableTabBar();
    void layout();

    virtual QSize sizeHint() const override;
    virtual void resizeEvent(QResizeEvent * event) override;
    virtual void tabLayoutChange() override;

signals:
    void addTabRequested();
    void tabRenamed(int index, const QString& name);

private:
    QPushButton* button;
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
