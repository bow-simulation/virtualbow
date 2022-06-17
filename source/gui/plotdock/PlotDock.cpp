#include "PlotDock.hpp"
#include <QLabel>

PlotDock::PlotDock() {
    placeholder = new QLabel();
    placeholder->setPixmap(QPixmap(":/icons/background.png"));
    placeholder->setAlignment(Qt::AlignCenter);
    placeholder->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));

    this->setWindowTitle("Graph");
    this->setObjectName("PlotView");    // Required to save state of main window
    this->setFeatures(QDockWidget::NoDockWidgetFeatures);
    this->setStyleSheet("#PlotView { background-image:url(:/icons/background.png); background-position: center; background-repeat: no-repeat; }");
    this->setWidget(placeholder);
}

void PlotDock::showPlot(QWidget* plot) {
    if(plot == nullptr) {
        this->setWidget(placeholder);
    }
    else {
        this->setWidget(plot);
    }
}
