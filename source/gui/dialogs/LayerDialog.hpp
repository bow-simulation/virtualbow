#pragma once
#include "solver/model//input/Layers.hpp"
#include "gui/PersistentDialog.hpp"
#include "gui/EditableTabBar.hpp"
#include "gui/editors/LayerEditor.hpp"

class LayerDialog: public PersistentDialog
{
    Q_OBJECT

public:
    LayerDialog(QWidget* parent);

    void setData(const std::vector<Layer>& layers);
    std::vector<Layer> getData() const;

signals:
    void modified();

private:
    EditableTabBar* tabs;

    LayerEditor* createEmptyTab();
    LayerEditor* createDefaultTab();
};
