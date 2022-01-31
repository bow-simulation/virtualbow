#include "PlotView.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include "gui/widgets/PlotWidget.hpp"

PlotView::PlotView(DataViewModel* model) {
    this->setObjectName("PlotView");    // Required to save state of main window
    this->setFeatures(QDockWidget::NoDockWidgetFeatures);
    this->setWindowTitle("Graph");
    this->setWidget(new PlotWidget());
}
