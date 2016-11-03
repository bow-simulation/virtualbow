#pragma once
#include "Plot.hpp"
#include "../model/InputData.hpp"
#include "../model/InputData.hpp"
#include "../numerics/Series.hpp"
#include "../numerics/CubicSpline.hpp"
#include "../numerics/ArcCurve.hpp"

class SplineView: public Plot
{
public:
    SplineView(const QString& x_label, const QString& y_label, DocItem<Series>& doc_item)
    {
        this->xAxis->setLabel(x_label);
        this->yAxis->setLabel(y_label);
        this->addGraph();
        this->addGraph();

        graph(1)->setLineStyle(QCPGraph::lsNone);
        graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, Qt::red, 6));

        connection = doc_item.connect([this](const Series& input)
        {
            try
            {
                CubicSpline spline(input);
                Series output = spline.sample(100);

                auto x_out = QVector<double>::fromStdVector(output.args());
                auto y_out = QVector<double>::fromStdVector(output.vals());
                this->graph(0)->setData(x_out, y_out);

                auto x_in = QVector<double>::fromStdVector(input.args());
                auto y_in = QVector<double>::fromStdVector(input.vals());
                this->graph(1)->setData(x_in, y_in);
            }
            catch(const std::runtime_error&)
            {
                this->graph(0)->setData(QVector<double>(), QVector<double>());
                this->graph(1)->setData(QVector<double>(), QVector<double>());
            }

            this->rescaleAxes();
            this->includeOrigin();
            this->replot();
        });
    }

private:
    Connection connection;
};

class ProfileView: public Plot
{
public:
    ProfileView(InputData& data)
        : data(data)
    {
        this->xAxis->setVisible(false);
        this->xAxis2->setVisible(true);
        this->xAxis2->grid()->setVisible(true);
        this->xAxis2->setLabel("y");
        this->yAxis->setLabel("x");
        this->yAxis->setRangeReversed(true);

        profile_curve = new QCPCurve(this->yAxis, this->xAxis2);
        profile_segments = new QCPCurve(this->yAxis, this->xAxis2);
        profile_segments->setLineStyle(QCPCurve::lsNone);
        profile_segments->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, Qt::red, 6));
        profile_segments->setScatterSkip(0);    // Todo: Why only for curves necessary?

        // Todo: Use std::bind?
        // Todo: Inefficient and ugly
        connections.push_back(data.profile_curvature.connect([this](const Series&){ update(); }));
        connections.push_back(data.profile_offset_x.connect([this](const double&){ update(); }));
        connections.push_back(data.profile_offset_y.connect([this](const double&){ update(); }));
        connections.push_back(data.profile_angle.connect([this](const double&){ update(); }));
        update();
    }

private:
    InputData& data;
    std::vector<Connection> connections;
    QCPCurve* profile_curve;    // Todo: Better naming?
    QCPCurve* profile_segments;

    void update()
    {
        try
        {
            ArcCurve profile(data.profile_curvature,
                             data.profile_offset_x,
                             data.profile_offset_y,
                             data.profile_angle,
                             20);

            ArcCurve segments(data.profile_curvature,
                              data.profile_offset_x,
                              data.profile_offset_y,
                              data.profile_angle,
                              0);

            auto xp = QVector<double>::fromStdVector(profile.x);
            auto yp = QVector<double>::fromStdVector(profile.y);

            auto xs = QVector<double>::fromStdVector(segments.x);
            auto ys = QVector<double>::fromStdVector(segments.y);

            profile_curve->setData(xp, yp);
            profile_segments->setData(xs, ys);
        }
        catch(const std::runtime_error&)
        {
            profile_curve->setData(QVector<double>(), QVector<double>());
            profile_segments->setData(QVector<double>(), QVector<double>());
        }

        this->rescaleAxes();
        this->includeOrigin();    // Todo
        this->yAxis->setScaleRatio(this->xAxis2);
        this->replot();
    }

    virtual void resizeEvent(QResizeEvent *event) override
    {
        this->yAxis->setScaleRatio(this->xAxis2);
        Plot::resizeEvent(event);
    }
};
