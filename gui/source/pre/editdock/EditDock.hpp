#pragma once
#include <QDockWidget>

class QStackedWidget;

class EditDock: public QDockWidget {
public:
    EditDock();
    void showEditor(QWidget* editor);

private:
    QWidget* placeholder;
};
