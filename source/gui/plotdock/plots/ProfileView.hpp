#pragma once
#include "gui/widgets/PlotWidget.hpp"
#include "gui/viewmodel/units/Quantity.hpp"
#include "solver/model/profile/ProfileInput.hpp"
#include "solver/numerics/EigenTypes.hpp"

class ProfileView: public PlotWidget
{
public:
    ProfileView(const Quantity& xy_quantity);
    void setData(const ProfileInput& data);
    void setSelection(const QList<int>& indices);

private:
    const Quantity& quantity;

    QAction* action_show_curvature;
    QAction* action_show_nodes;

    ProfileInput input;
    QList<int> selection;

    QList<QCPCurve*> segment_curves;
    QList<QCPCurve*> segment_nodes;    // TODO: Use some item type for this?
    QList<QCPCurve*> curvature_lines;

    void updatePlot();
    void updateSelection();
    void updateVisibility();

    //void setCurvatureVisible(bool visible);
    //void setNodesVisible(bool visible);
};
