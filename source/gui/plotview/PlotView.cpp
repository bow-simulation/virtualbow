#include "PlotView.hpp"
#include <QStackedWidget>
#include <QLabel>

PlotView::PlotView()
    : stack(new QStackedWidget()),
      placeholder(new QLabel("Placeholder"))
{
    this->setWindowTitle("Plot");
    this->setObjectName("PropertyView");    // Required to save state of main window
    this->setFeatures(QDockWidget::NoDockWidgetFeatures);
    this->setWidget(placeholder);
}

void PlotView::showPlot(QWidget* plot) {
    if(plot == nullptr) {
        this->setWidget(placeholder);
    }
    else {
        this->setWidget(plot);
    }
}
