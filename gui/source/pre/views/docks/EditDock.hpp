#pragma once
#include <QDockWidget>

class QItemSelectionModel;
class MainModel;

class EditDock: public QDockWidget {
public:
    EditDock(MainModel* viewModel);
    void showEditor(QWidget* editor);

private:
    QWidget* placeholder;
};
