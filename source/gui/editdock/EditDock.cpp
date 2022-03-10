#include "EditDock.hpp"
#include <QTableView>

EditDock::EditDock()
    : placeholder(new QTableView())    // Show an empty table view by default
{
    this->setWindowTitle("Properties");
    this->setObjectName("EditDock");    // Required to save state of main window
    this->setFeatures(QDockWidget::NoDockWidgetFeatures);
    this->setWidget(placeholder);
}

void EditDock::showEditor(QWidget* editor) {
    this->setWidget(editor);
    if(editor == nullptr) {
        this->setWidget(placeholder);
    }
    else {
        this->setWidget(editor);
    }
}
