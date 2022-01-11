#include "TreeView.hpp"
#include "TreeModel.hpp"
#include "gui/viewmodel/ViewModel.hpp"

TreeView::TreeView(ViewModel* model) {
    setModel(new TreeModel(model));
    setHeaderHidden(true);
}
