#include "SplineView.hpp"
#include "../../model/InputData.hpp"
#include "../../numerics/CubicSpline.hpp"

SplineView::SplineView(const QString& lbx, const QString& lby, DocItem<Series>& doc_item)
    : Plot(lbx, lby)
{
    this->addSeries({}, Style::Scatter(QCPScatterStyle::ssDisc, Qt::red));
    this->addSeries({}, Style::Line(Qt::blue));

    connection = doc_item.connect([this](const Series& input)
    {
        try
        {
            Series output = CubicSpline::sample(input, 150);    // Todo: Magic number
            this->setData(0, input);
            this->setData(1, output);
        }
        catch(const std::runtime_error&)
        {
            this->setData(0, Series());
            this->setData(1, Series());
        }

        this->fitContent(false, true);
        this->replot();
    });
}
