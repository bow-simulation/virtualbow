#pragma once
#include "gui/PersistentDialog.hpp"
#include "gui/editors/LayerEditor.hpp"
#include "gui/units/UnitSystem.hpp"

class WidthDialog: public PersistentDialog
{
    Q_OBJECT

public:
    WidthDialog(QWidget* parent);

    MatrixXd getData() const;
    void setData(const MatrixXd& width);

signals:
    void modified();

private:
    TableEditor* edit;
    SplineView* view;
};
