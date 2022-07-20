#include "PlotDock.hpp"
#include <QLabel>

class PlaceholderLabel: public QLabel {
public:
    PlaceholderLabel() {
        setObjectName("PlaceholderLabel");
        setStyleSheet("#PlaceholderLabel { background-image:url(:/icons/background.png); background-position: center; background-repeat: no-repeat; }");
    }

    QSize sizeHint() const override {
        return {256, 256};    // Size of the background image. Only relevant on first launch, before the dock was resized by the user.
    }
};

PlotDock::PlotDock() {
    placeholder = new PlaceholderLabel();

    this->setWindowTitle("Graph");
    this->setObjectName("PlotView");    // Required to save state of main window
    this->setFeatures(QDockWidget::NoDockWidgetFeatures);
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
