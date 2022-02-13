#include "PropertyView.hpp"
#include <QStackedWidget>
#include <QLabel>

PropertyView::PropertyView()
    : stack(new QStackedWidget()),
      placeholder(new QLabel("Placeholder"))
{
    this->setWindowTitle("Properties");
    this->setObjectName("PropertyView");    // Required to save state of main window
    this->setFeatures(QDockWidget::NoDockWidgetFeatures);
    this->setWidget(placeholder);
}

void PropertyView::showEditor(QWidget* editor) {
    if(editor == nullptr) {
        this->setWidget(placeholder);
    }
    else {
        this->setWidget(editor);
    }
}
