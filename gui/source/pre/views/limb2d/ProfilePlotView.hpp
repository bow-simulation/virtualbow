#pragma once
#include "pre/widgets/PlotWidget.hpp"

class MainModel;

class ProfilePlotView: public PlotWidget
{
public:
    ProfilePlotView(MainModel* model);
    /*
    void setData(const Profile& data);
    void setSelection(const QList<int>& indices);
    */

private:
    MainModel* model;

    QAction* action_show_curvature;
    QAction* action_show_nodes;

    QCPCurve* curveLine;
    QCPCurve* curvePoints;
    QCPCurve* curveSelected;

    void updatePlot();
    void updateSelection();
    void updateVisibility();

    //void setCurvatureVisible(bool visible);
    //void setNodesVisible(bool visible);
};
