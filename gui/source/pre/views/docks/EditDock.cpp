#include "EditDock.hpp"
#include "pre/models/MainModel.hpp"
#include "pre/models/MainTreeModel.hpp"
#include "pre/views/CommentsView.hpp"
#include "pre/views/SettingsView.hpp"
#include "pre/views/HandleView.hpp"
#include "pre/views/DrawView.hpp"
#include "pre/views/MaterialView.hpp"
#include "pre/views/LayerView.hpp"
#include "pre/views/WidthView.hpp"
#include "pre/views/StringView.hpp"
#include "pre/views/MassesView.hpp"
#include "pre/views/DampingView.hpp"
#include "pre/views/LineView.hpp"
#include "pre/views/ArcView.hpp"
#include "pre/views/SpiralView.hpp"
#include "pre/views/SplineView.hpp"
#include "pre/widgets/ScrollArea.hpp"
#include <QTableView>

EditDock::EditDock(MainModel* viewModel)
    : placeholder(new QTableView())    // Show an empty table view by default
{
    setObjectName("EditDock2");    // Required to save state of main window // TODO: Remove 2 after one release cycle?
    setFeatures(QDockWidget::NoDockWidgetFeatures);
    setWindowTitle("Properties");

    auto selectionModel = viewModel->getModelTreeSelectionModel();
    QObject::connect(selectionModel, &QItemSelectionModel::selectionChanged, this, [=, this]{
        QModelIndexList selection = selectionModel->selectedIndexes();

        if(selection.size() == 1 && selection.first().internalId() == ItemType::TOPLEVEL && selection.first().row() == TopLevelItem::COMMENTS) {
            auto model = viewModel->getCommentsModel();
            auto editor = new CommentsView(model);
            showEditor(editor);
            return;
        }

        if(selection.size() == 1 && selection.first().internalId() == ItemType::TOPLEVEL && selection.first().row() == TopLevelItem::SETTINGS) {
            auto model = viewModel->getSettingsModel();
            auto editor = new SettingsView(model);
            showEditor(editor);
            return;
        }

        if(selection.size() == 1 && selection.first().internalId() == ItemType::TOPLEVEL && selection.first().row() == TopLevelItem::DRAW) {
            auto model = viewModel->getDrawModel();
            auto editor = new DrawView(model);
            showEditor(editor);
            return;
        }

        if(selection.size() == 1 && selection.first().internalId() == ItemType::TOPLEVEL && selection.first().row() == TopLevelItem::HANDLE) {
            auto model = viewModel->getHandleModel();
            auto editor = new HandleView(model);
            showEditor(editor);
            return;
        }

        if(selection.size() == 1 && selection.first().internalId() == ItemType::MATERIAL) {
            auto model = viewModel->getMaterialModel(selection.first().row());
            auto editor = new MaterialView(model);
            showEditor(editor);
            return;
        }

        if(selection.size() == 1 && selection.first().internalId() == ItemType::LAYER) {
            auto model = viewModel->getLayerModel(selection.first().row());
            auto tableModel = viewModel->getLayerHeightModel(selection.first().row());
            auto editor = new LayerView(model, tableModel);
            showEditor(editor);
            return;
        }

        if(selection.size() == 1 && selection.first().internalId() == ItemType::SEGMENT) {
            auto lineModel = viewModel->getLineModel(selection.first().row());
            if(lineModel != nullptr) {
                auto editor = new LineView(lineModel);
                showEditor(editor);
                return;
            }

            auto arcModel = viewModel->getArcModel(selection.first().row());
            if(arcModel != nullptr) {
                auto editor = new ArcView(arcModel);
                showEditor(editor);
                return;
            }

            auto spiralModel = viewModel->getSpiralModel(selection.first().row());
            if(spiralModel != nullptr) {
                auto editor = new SpiralView(spiralModel);
                showEditor(editor);
                return;
            }

            auto splineModel = viewModel->getSplineModel(selection.first().row());
            if(splineModel != nullptr) {
                auto editor = new SplineView(splineModel);
                showEditor(editor);
                return;
            }
        }

        if(selection.size() == 1 && selection.first().internalId() == ItemType::TOPLEVEL && selection.first().row() == TopLevelItem::WIDTH) {
            auto model = viewModel->getWidthModel();
            auto editor = new WidthView(model);
            showEditor(editor);
            return;
        }

        if(selection.size() == 1 && selection.first().internalId() == ItemType::TOPLEVEL && selection.first().row() == TopLevelItem::STRING) {
            auto model = viewModel->getStringModel();
            auto editor = new StringView(model);
            showEditor(editor);
            return;
        }

        if(selection.size() == 1 && selection.first().internalId() == ItemType::TOPLEVEL && selection.first().row() == TopLevelItem::MASSES) {
            auto model = viewModel->getMassesModel();
            auto editor = new MassesView(model);
            showEditor(editor);
            return;
        }

        if(selection.size() == 1 && selection.first().internalId() == ItemType::TOPLEVEL && selection.first().row() == TopLevelItem::DAMPING) {
            auto model = viewModel->getDampingModel();
            auto editor = new DampingView(model);
            showEditor(editor);
            return;
        }

        showEditor(nullptr);
    });

    showEditor(nullptr);
}

void EditDock::showEditor(QWidget* editor) {
    if(editor != nullptr) {
        setWidget(scrollArea(editor));
    }
    else {
        setWidget(placeholder);
    }
}
