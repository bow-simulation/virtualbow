#pragma once
#include <QDockWidget>

class QItemSelectionModel;
class MainVM;

class EditDock: public QDockWidget {
public:
    EditDock(MainVM* viewModel);
    void showEditor(QWidget* editor);

private:
    QWidget* placeholder;
};
