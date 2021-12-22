#pragma once
#include "gui/widgets/PersistentDialog.hpp"
#include "gui/widgets/TableEditor.hpp"
#include "gui/widgets/SplineView.hpp"
#include "gui/units/UnitSystem.hpp"

class WidthDialog: public PersistentDialog
{
    Q_OBJECT

public:
    WidthDialog(QWidget* parent);

    std::vector<Vector<2>> getData() const;
    void setData(const std::vector<Vector<2>>& width);

signals:
    void modified();

private:
    TableEditor* edit;
    SplineView* view;
};
