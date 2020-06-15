#pragma once
#include "solver/model/input/InputData.hpp"
#include "gui/editors/TableEditor.hpp"
#include "gui/editors/DoubleEditor.hpp"
#include "gui/views/SplineView.hpp"
#include "gui/views/LayerColors.hpp"
#include "gui/EditableTabBar.hpp"

class LayerEditor: public QWidget
{
    Q_OBJECT

public:
    LayerEditor(EditableTabBar* tabs);

    Layer getData() const;
    void setData(const Layer& layer);

signals:
    void modified();

private:
    EditableTabBar* tabs;
    TableEditor* table;
    SplineView* view;
    DoubleEditor* edit_rho;
    DoubleEditor* edit_E;

    void updateTabIcon();
};
