#include "ProfileView.hpp"
#include "model/input/InputData.hpp"
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

    // Event handling
    QObject::connect(&data.profile.segments, &DocumentNode::modified, this, &ProfileView::update);
    QObject::connect(&data.profile.x0,       &DocumentNode::modified, this, &ProfileView::update);
    QObject::connect(&data.profile.y0,       &DocumentNode::modified, this, &ProfileView::update);
    QObject::connect(&data.profile.phi0,     &DocumentNode::modified, this, &ProfileView::update);

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
        Curve2D curve = ArcCurve::sample(data.profile.segments,
                                         data.profile.x0,
                                         data.profile.y0,
                                         data.profile.phi0,
                                         150);  // Todo: Magic number

        Curve2D nodes = ArcCurve::nodes(data.profile.segments,
                                        data.profile.x0,
                                        data.profile.y0,
                                        data.profile.phi0);

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
