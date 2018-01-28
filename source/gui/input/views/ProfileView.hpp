#pragma once
#include "gui/PlotWidget.hpp"
#include "bow/input/InputData.hpp"
#include "numerics/ArcCurve.hpp"

class ProfileView: public PlotWidget
{
public:
    ProfileView()
    {
        this->xAxis->setLabel("X [m]");
        this->yAxis->setLabel("Y [m]");
        this->setAspectPolicy(PlotWidget::SCALE_Y);

        // Line
        curve0 = new QCPCurve(this->xAxis, this->yAxis);
        curve0->setPen({Qt::blue, 2});
        curve0->setScatterSkip(0);    // Having to explicitly state this is retarded

        // Nodes
        curve1 = new QCPCurve(this->xAxis, this->yAxis);
        curve1->setScatterStyle({QCPScatterStyle::ssSquare, Qt::red, 8});
        curve1->setLineStyle(QCPCurve::lsNone);
        curve1->setScatterSkip(0);    // Todo: Having to explicitly state this is retarded
    }

    void setData(Profile profile)
    {
        try
        {
            Curve2D curve = ArcCurve::sample(profile.segments, profile.x0, profile.y0, profile.phi0, 150);  // Magic number
            Curve2D nodes = ArcCurve::nodes(profile.segments, profile.x0, profile.y0, profile.phi0);

            curve0->setData(curve.x, curve.y);
            curve1->setData(nodes.x, nodes.y);
        }
        catch(const std::runtime_error&)
        {
            curve0->data()->clear();
            curve1->data()->clear();
        }

        this->rescaleAxes();
        this->replot();
    }

private:
    QCPCurve* curve0;
    QCPCurve* curve1;
};
