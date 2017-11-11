#include "ProfileView.hpp"
#include "model/InputData.hpp"
#include "numerics/ArcCurve.hpp"

ProfileView::ProfileView(InputData& data)
    : PlotWidget({750, 250}),   // Todo: Magic number
      data(data)
{
    this->xAxis->setLabel("X [m]");
    this->yAxis->setLabel("Y [m]");
    this->setAspectPolicy(PlotWidget::SCALE_Y);

    // Line
    curve0 = new QCPCurve(this->xAxis, this->yAxis);
    curve0->setPen({Qt::blue, 2});
    curve0->setScatterSkip(0);    // Todo: Having to explicitly state this is retarded

    // Segment nodes
    curve1 = new QCPCurve(this->xAxis, this->yAxis);
    curve1->setScatterStyle({QCPScatterStyle::ssSquare, Qt::blue, 8});
    curve1->setLineStyle(QCPCurve::lsNone);
    curve1->setScatterSkip(0);    // Todo: Having to explicitly state this is retarded

    // Todo: Use std::bind?
    // Todo: Inefficient and ugly
    data.profile_segments.on_value_changed([&]{ update(); });
    data.profile_x0.on_value_changed      ([&]{ update(); });
    data.profile_y0.on_value_changed      ([&]{ update(); });
    data.profile_phi0.on_value_changed    ([&]{ update(); });
    update();
}

void ProfileView::setSelection(const std::vector<int>& indices)
{
    // Todo: Do something
}

void ProfileView::update()
{
    try
    {
        Curve2D curve = ArcCurve::sample(data.profile_segments,
                                         data.profile_x0,
                                         data.profile_y0,
                                         data.profile_phi0,
                                         150);  // Todo: Magic number

        Curve2D nodes = ArcCurve::nodes(data.profile_segments,
                                        data.profile_x0,
                                        data.profile_y0,
                                        data.profile_phi0);

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
