#pragma once
#include "solver/model/input/InputData.hpp"
#include "gui/widgets/TableEditor.hpp"
#include "gui/widgets/DoubleEditor.hpp"
#include "gui/widgets/SplineView.hpp"
#include "gui/limbview/LayerColors.hpp"
#include "gui/widgets/EditableTabBar.hpp"

class LayerEditor: public QSplitter
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
