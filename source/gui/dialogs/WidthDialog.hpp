#pragma once
#include "gui/PersistentDialog.hpp"
#include "gui/editors/LayerEditor.hpp"

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
    SeriesEditor* edit;
    SplineView* view;
};
