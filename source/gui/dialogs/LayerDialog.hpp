#pragma once
#include "solver/model/input/Layers.hpp"
#include "gui/PersistentDialog.hpp"
#include "gui/EditableTabBar.hpp"
#include "gui/editors/LayerEditor.hpp"
#include "gui/units/UnitSystem.hpp"

class LayerDialog: public PersistentDialog
{
    Q_OBJECT

public:
    LayerDialog(QWidget* parent);

    std::vector<Layer> getData() const;
    void setData(const std::vector<Layer>& layers);
    void setUnits(const UnitSystem& units);

signals:
    void modified();

private:
    EditableTabBar* tabs;

    LayerEditor* createEmptyTab();
    LayerEditor* createDefaultTab();
};
