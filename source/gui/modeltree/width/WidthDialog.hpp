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

    MatrixXd getData() const;
    void setData(const MatrixXd& width);

signals:
    void modified();

private:
    TableEditor* edit;
    SplineView* view;
};
