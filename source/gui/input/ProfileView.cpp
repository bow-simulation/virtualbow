#include "ProfileView.hpp"
#include "../../model/InputData.hpp"
#include "../../numerics/ArcCurve.hpp"

ProfileView::ProfileView(InputData& data)
    : Plot("x [m]", "y [m]", Align::TopLeft),
      data(data)
{
    this->setExpansionMode(ExpansionMode::OneSided, ExpansionMode::OneSided);
    this->addSeries({}, Style::Line(Qt::blue, 2));
    this->addSeries({}, Style::Scatter(QCPScatterStyle::ssDisc, Qt::red));

    // Todo: Use std::bind?
    // Todo: Inefficient and ugly
    connections.push_back(data.profile_segments.connect([this](const Series&){ update(); }));
    connections.push_back(data.profile_x0.connect([this](const double&){ update(); }));
    connections.push_back(data.profile_y0.connect([this](const double&){ update(); }));
    connections.push_back(data.profile_phi0.connect([this](const double&){ update(); }));
    update();
}

void ProfileView::update()
{
    try
    {
        Curve profile = ArcCurve::sample(data.profile_segments,
                                         data.profile_x0,
                                         data.profile_y0,
                                         data.profile_phi0,
                                         150);  // Todo: Magic number

        Curve segments = ArcCurve::sample(data.profile_segments,
                                          data.profile_x0,
                                          data.profile_y0,
                                          data.profile_phi0,
                                          0);

        this->setData(0, Series(profile.x, profile.y));
        this->setData(1, Series(segments.x, segments.y));
    }
    catch(const std::runtime_error&)
    {
        this->setData(0, Series());
        this->setData(1, Series());
    }

    this->fitContent(true, false);
    this->replot();
}
