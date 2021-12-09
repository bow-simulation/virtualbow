#pragma once
#include "gui/PlotWidget.hpp"
#include "gui/units/UnitGroup.hpp"
#include "solver/model/profile/ProfileInput.hpp"
#include "solver/numerics/Eigen.hpp"
#include "solver/numerics/Series.hpp"

class ProfileView: public PlotWidget
{
public:
    ProfileView(const UnitGroup& xy_unit);
    void setData(const ProfileInput& data);
    void setSelection(const QVector<int>& indices);

private:
    const UnitGroup& unit;

    ProfileInput input;
    QVector<int> selection;

    QCPCurve* profile_curve;
    QCPCurve* profile_nodes_unselected;
    QCPCurve* profile_nodes_selected;

    QCPCurve* curvature_outline;
    QList<QCPCurve*> curvature_lines;

    void updatePlot();
    void setCurvatureVisible(bool visible);
    void setNodesVisible(bool visible);
};
