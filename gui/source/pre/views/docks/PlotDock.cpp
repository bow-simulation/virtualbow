#include "PlotDock.hpp"
#include "pre/models/MainModel.hpp"
#include "pre/models/MainTreeModel.hpp"
#include "pre/views/limb2d/WidthPlotView.hpp"
#include "pre/views/limb2d/HeightPlotView.hpp"
#include "pre/views/limb2d/ProfilePlotView.hpp"
#include <QItemSelectionModel>
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

PlotDock::PlotDock(MainModel* model) {
    placeholder = new PlaceholderLabel();

    this->setWindowTitle("Graph");
    this->setObjectName("PlotView");    // Required to save state of main window
    this->setFeatures(QDockWidget::NoDockWidgetFeatures);
    this->setWidget(placeholder);

    auto selectionModel = model->getModelTreeSelectionModel();
    QObject::connect(selectionModel, &QItemSelectionModel::selectionChanged, this, [=] {
        QModelIndexList selection = selectionModel->selectedIndexes();
        if(selection.size() == 1) {
            QPersistentModelIndex index(selection.first());

            if(index.internalId() == ItemType::LAYER) {
                showPlot(index, [=]{ return new HeightPlotView(model, index); });
                return;
            }

            if(index.internalId() == ItemType::TOPLEVEL && index.row() == TopLevelItem::WIDTH) {
                showPlot(index, [=]{ return new WidthPlotView(model); });
                return;
            }

            if((index.internalId() == ItemType::TOPLEVEL && index.row() == TopLevelItem::PROFILE) || index.internalId() == ItemType::SEGMENT) {
                showPlot(index, [=]{ return new ProfilePlotView(model); });
                return;
            }
        }

        showPlaceholder();
    });

    showPlaceholder();
}

void PlotDock::showPlaceholder() {
    setWidget(placeholder);
}

void PlotDock::showPlot(QPersistentModelIndex index, const std::function<QWidget*()>& create) {
    // Remove invalid model indices and delete their associated plots (for example after model reset)
    plots.removeIf([](std::pair<QPersistentModelIndex, QWidget*> pair) {
        if(!pair.first.isValid()) {
            pair.second->deleteLater();
            return true;
        }
        else {
            return false;
        }
    });

    // Check if a plot for the model index exists, create a new one if not
    if(!plots.contains(index)) {
        qInfo() << "Create, Size = " << plots.size();
        plots.insert(index, create());
    }

    // Show plot for model index
    setWidget(plots[index]);
}
