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
    LayerDialog(QWidget* parent, const UnitSystem& units);

    std::vector<Layer> getData() const;
    void setData(const std::vector<Layer>& layers);

signals:
    void modified();

private:
    EditableTabBar* tabs;
    const UnitSystem& units;

    LayerEditor* createEmptyTab();
    LayerEditor* createDefaultTab();
};
