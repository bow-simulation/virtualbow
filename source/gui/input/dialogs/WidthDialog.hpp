#pragma once
#include "gui/PersistentDialog.hpp"
#include "gui/input/editors/LayerEditor.hpp"
#include "bow/input/InputData.hpp"

class WidthDialog: public PersistentDialog
{
    Q_OBJECT

public:
    WidthDialog(QWidget* parent);

    Series getData() const;
    void setData(const Series& width);

signals:
    void modified();

private:
    SeriesEditor* edit;
    SplineView* view;
};
