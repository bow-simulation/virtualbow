#pragma once
#include "bow/input/InputData.hpp"
#include "gui/input/editors/SeriesEditor.hpp"
#include "gui/input/editors/DoubleEditor.hpp"
#include "gui/input/views/SplineView.hpp"
#include "gui/input/views/LayerColors.hpp"
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
    SeriesEditor* table;
    SplineView* view;
    DoubleEditor* edit_rho;
    DoubleEditor* edit_E;

    void updateTabIcon();
};
