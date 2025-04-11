#include "EditDock.hpp"
#include "pre/viewmodels/MainVM.hpp"
#include "pre/viewmodels/ModelTreeVM.hpp"
#include "pre/views/CommentsView.hpp"
#include "pre/views/SettingsView.hpp"
#include "pre/views/DimensionsView.hpp"
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
#include "pre/utils/ScrollArea.hpp"
#include <QTableView>

EditDock::EditDock(MainVM* viewModel)
    : placeholder(new QTableView())    // Show an empty table view by default
{
    setObjectName("EditDock");    // Required to save state of main window
    setFeatures(QDockWidget::NoDockWidgetFeatures);
    setWindowTitle("Properties");

    auto selectionModel = viewModel->getModelTreeSelectionVM();
    QObject::connect(selectionModel, &QItemSelectionModel::selectionChanged, this, [=]{
        QModelIndexList selection = selectionModel->selectedIndexes();

        if(selection.size() == 1 && selection.first().internalId() == ItemType::TOPLEVEL && selection.first().row() == TopLevelItem::COMMENTS) {
            auto model = viewModel->getCommentsVM();
            auto editor = new CommentsView(model);
            showEditor(editor);
            return;
        }

        if(selection.size() == 1 && selection.first().internalId() == ItemType::TOPLEVEL && selection.first().row() == TopLevelItem::SETTINGS) {
            auto model = viewModel->getSettingsVM();
            auto editor = new SettingsView(model);
            showEditor(editor);
            return;
        }

        if(selection.size() == 1 && selection.first().internalId() == ItemType::TOPLEVEL && selection.first().row() == TopLevelItem::DIMENSIONS) {
            auto model = viewModel->getDimensionsVM();
            auto editor = new DimensionsView(model);
            showEditor(editor);
            return;
        }

        if(selection.size() == 1 && selection.first().internalId() == ItemType::MATERIAL) {
            auto model = viewModel->getMaterialVM(selection.first().row());
            auto editor = new MaterialView(model);
            showEditor(editor);
            return;
        }

        if(selection.size() == 1 && selection.first().internalId() == ItemType::LAYER) {
            auto model = viewModel->getLayerVM(selection.first().row());
            auto tableModel = viewModel->getLayerHeightVM(selection.first().row());
            auto editor = new LayerView(model, tableModel);
            showEditor(editor);
            return;
        }

        if(selection.size() == 1 && selection.first().internalId() == ItemType::SEGMENT) {
            auto lineVM = viewModel->getLineVM(selection.first().row());
            if(lineVM != nullptr) {
                auto editor = new LineView(lineVM);
                showEditor(editor);
                return;
            }

            auto arcVM = viewModel->getArcVM(selection.first().row());
            if(arcVM != nullptr) {
                auto editor = new ArcView(arcVM);
                showEditor(editor);
                return;
            }

            auto spiralVM = viewModel->getSpiralVM(selection.first().row());
            if(spiralVM != nullptr) {
                auto editor = new SpiralView(spiralVM);
                showEditor(editor);
                return;
            }

            auto splineVM = viewModel->getSplineVM(selection.first().row());
            if(splineVM != nullptr) {
                auto editor = new SplineView(splineVM);
                showEditor(editor);
                return;
            }
        }

        if(selection.size() == 1 && selection.first().internalId() == ItemType::TOPLEVEL && selection.first().row() == TopLevelItem::WIDTH) {
            auto model = viewModel->getWidthVM();
            auto editor = new WidthView(model);
            showEditor(editor);
            return;
        }

        if(selection.size() == 1 && selection.first().internalId() == ItemType::TOPLEVEL && selection.first().row() == TopLevelItem::STRING) {
            auto model = viewModel->getStringVM();
            auto editor = new StringView(model);
            showEditor(editor);
            return;
        }

        if(selection.size() == 1 && selection.first().internalId() == ItemType::TOPLEVEL && selection.first().row() == TopLevelItem::MASSES) {
            auto model = viewModel->getMassesVM();
            auto editor = new MassesView(model);
            showEditor(editor);
            return;
        }

        if(selection.size() == 1 && selection.first().internalId() == ItemType::TOPLEVEL && selection.first().row() == TopLevelItem::DAMPING) {
            auto model = viewModel->getDampingVM();
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
