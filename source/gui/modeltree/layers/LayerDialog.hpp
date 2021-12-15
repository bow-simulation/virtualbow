#pragma once
#include "LayerEditor.hpp"
#include "solver/model/input/Layers.hpp"
#include "gui/widgets/PersistentDialog.hpp"
#include "gui/widgets/EditableTabBar.hpp"
#include "gui/units/UnitSystem.hpp"

class LayerDialog: public PersistentDialog {
    Q_OBJECT

public:
    LayerDialog(QWidget* parent);

    std::vector<Layer> getData() const;
    void setData(const std::vector<Layer>& layers);

signals:
    void modified();

private:
    EditableTabBar* tabs;

    LayerEditor* createEmptyTab();
    LayerEditor* createDefaultTab();
};
