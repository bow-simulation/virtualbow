#pragma once
#include "qcustomplot/qcustomplot.h"
#include "../model/Document.hpp"
#include "../numerics/Series.hpp"
#include "../numerics/CubicSpline.hpp"

class Plot: public QWidget
{
public:
    enum class Align{ BottomLeft, BottomRight, TopLeft, TopRight };

    Plot(const QString& lbx, const QString& lby, Align align = Align::BottomLeft)
        : plot(new QCustomPlot),
          include_origin_x(false),
          include_origin_y(false),
          fix_aspect_ratio(false)
    {
        switch(align)
        {
        case Align::BottomLeft:
            x_axis = plot->xAxis;
            y_axis = plot->yAxis;
            x_axis->setRangeReversed(false);
            y_axis->setRangeReversed(false);
            break;

        case Align::BottomRight:
            x_axis = plot->xAxis;
            y_axis = plot->yAxis2;
            x_axis->setRangeReversed(true);
            y_axis->setRangeReversed(false);
            plot->yAxis->setVisible(false);
            break;

        case Align::TopLeft:
            x_axis = plot->xAxis2;
            y_axis = plot->yAxis;
            x_axis->setRangeReversed(false);
            y_axis->setRangeReversed(true);
            plot->xAxis->setVisible(false);
            break;

        case Align::TopRight:
            x_axis = plot->xAxis2;
            y_axis = plot->yAxis2;
            x_axis->setRangeReversed(true);
            y_axis->setRangeReversed(true);
            plot->xAxis->setVisible(false);
            plot->yAxis->setVisible(false);
            break;
        }

        x_axis->setVisible(true);
        y_axis->setVisible(true);

        x_axis->grid()->setVisible(true);
        y_axis->grid()->setVisible(true);

        x_axis->setLabel(lbx);
        y_axis->setLabel(lby);

        x_axis->setUpperEnding(QCPLineEnding::esFlatArrow);
        y_axis->setUpperEnding(QCPLineEnding::esFlatArrow);

        auto vbox = new QVBoxLayout();
        vbox->addWidget(plot);
        setLayout(vbox);

        //setInteraction(QCP::iRangeDrag, true);
        //setInteraction(QCP::iRangeZoom, true);

        QObject::connect(plot, &QCustomPlot::beforeReplot, [this]()
        {
            // Todo: Get rid of this function call and instead just calculate the axis ranges without applying them
            // This way a rescale is not done every plot which makes enabling the user interactions above feasible
            plot->rescaleAxes();

            auto expand_axis = [this](QCPAxis* axis)
            {
                auto r = axis->range();
                r.expand(0.0);
                axis->setRange(r);
            };

            if(include_origin_x)
            {
                expand_axis(x_axis);
            }

            if(include_origin_y)
            {
                expand_axis(y_axis);
            }

            if(fix_aspect_ratio)
            {
                double w = plot->axisRect(0)->width();
                double h = plot->axisRect(0)->height();

                double lx = x_axis->range().size();
                double ly = y_axis->range().size();

                if(h/w > ly/lx)
                {
                    ly = h/w*lx;
                    y_axis->setRange(y_axis->range().lower, y_axis->range().lower + ly);
                }
                else
                {
                    lx = w/h*ly;
                    x_axis->setRange(x_axis->range().lower, x_axis->range().lower + lx);
                }
            }
        });

        // Context menu

        plot->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(plot, &QCustomPlot::customContextMenuRequested, [this](QPoint pos)
        {
            QMenu menu(this);
            menu.addAction("Export...", [this](){ exportDialog(); });   // Todo: Use std::bind?
            menu.exec(plot->mapToGlobal(pos));
        });

    }

    void addSeries()
    {
        auto curve = new QCPCurve(x_axis, y_axis);
        curve->setScatterSkip(0);
        series.push_back(curve);
    }

    void setData(size_t i, Series data)
    {
        auto x = QVector<double>::fromStdVector(data.args());
        auto y = QVector<double>::fromStdVector(data.vals());
        series[i]->setData(x, y);
    }

    void setLineStyle(size_t i, QCPCurve::LineStyle style)
    {
        series[i]->setLineStyle(style);
    }

    void setScatterStyle(size_t i, QCPScatterStyle style)
    {
        series[i]->setScatterStyle(style);
    }

    void replot()
    {
        plot->replot();
    }

    void includeOrigin(bool x, bool y)
    {
        include_origin_x = x;
        include_origin_y = y;
    }

    void fixAspectRatio(bool value)
    {
        fix_aspect_ratio = value;
    }

    QCPAxis* axisX()
    {
        return x_axis;
    }

    QCPAxis* axisY()
    {
        return y_axis;
    }

private:
    QCustomPlot* plot;
    QCPAxis* x_axis;
    QCPAxis* y_axis;

    std::vector<QCPCurve*> series;
    bool include_origin_x;
    bool include_origin_y;
    bool fix_aspect_ratio;

    virtual void resizeEvent(QResizeEvent *event) override
    {
        plot->replot();
    }

    void exportDialog()
    {
        const char* PNG_FILE  = "PNG image (*.png)";
        const char* BMP_FILE  = "BMP image (*.bmp)";
        const char* PDF_FILE  = "Portable Document Format (*.pdf)";

        QFileDialog dialog(this);
        dialog.setAcceptMode(QFileDialog::AcceptSave);

        QStringList filters;
        filters << PNG_FILE << BMP_FILE << PDF_FILE;
        dialog.setNameFilters(filters);

        // Todo: Is there a better way to connect default suffix to the selected name filter?
        QObject::connect(&dialog, &QFileDialog::filterSelected, [&](const QString &filter)
        {
            if(filter == PNG_FILE)
                dialog.setDefaultSuffix(".png");

            if(filter == BMP_FILE)
                dialog.setDefaultSuffix(".bmp");

            if(filter == PDF_FILE)
                dialog.setDefaultSuffix(".pdf");
        });
        dialog.filterSelected(PNG_FILE);

        if(dialog.exec() == QDialog::Accepted)
        {
            QString filter = dialog.selectedNameFilter();
            QString path = dialog.selectedFiles().first();

            if(filter == PNG_FILE)
                plot->savePng(path);

            if(filter == BMP_FILE)
                plot->saveBmp(path);

            if(filter == PDF_FILE)
                plot->savePdf(path);
        }
    }
};
