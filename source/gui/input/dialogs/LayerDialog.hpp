#pragma once
#include "bow/input/Layers.hpp"
#include "gui/PersistentDialog.hpp"
#include "gui/EditableTabBar.hpp"
#include "gui/input/editors/LayerEditor.hpp"

class LayerDialog: public PersistentDialog
{
    Q_OBJECT

public:
    LayerDialog(QWidget* parent);

    void setData(const Layers& layers);
    Layers getData() const;

signals:
    void modified();

private:
    EditableTabBar* tabs;

    LayerEditor* createEmptyTab();
    LayerEditor* createDefaultTab();
};
