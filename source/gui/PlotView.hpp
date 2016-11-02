#pragma once
#include "Plot.hpp"
#include "../model/Document.hpp"
#include "../numerics/Series.hpp"
#include "../numerics/CubicSpline.hpp"

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
            catch(std::runtime_error e)
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
    DocItem<Series>::Connection connection;
};
