#pragma once
#include <QDockWidget>

class QStackedWidget;

class PropertyView: public QDockWidget {
public:
    PropertyView();
    void showEditor(QWidget* editor);

private:
    QStackedWidget* stack;
    QWidget* placeholder;
};
