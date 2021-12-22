#pragma once
#include "gui/widgets/PlotWidget.hpp"
#include "gui/units/UnitGroup.hpp"
#include "solver/model/profile/ProfileInput.hpp"
#include "solver/numerics/EigenTypes.hpp"
#include "solver/numerics/Series.hpp"

class ProfileView: public PlotWidget
{
public:
    ProfileView(const UnitGroup& xy_unit);
    void setData(const ProfileInput& data);
    void setSelection(const QList<int>& indices);

private:
    const UnitGroup& unit;

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
