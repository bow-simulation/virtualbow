#include "Plot.hpp"
#include "../numerics/Series.hpp"

Plot::Plot(const QString& lbx, const QString& lby, Align align)
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

    //plot->setInteraction(QCP::iRangeDrag, true);
    //plot->setInteraction(QCP::iRangeZoom, true);
    plot->setInteraction(QCP::iSelectPlottables, true);

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
    QObject::connect(plot, &QCustomPlot::customContextMenuRequested, [&](QPoint pos)
    {
        auto menu = new QMenu(this);
        menu->addAction("Copy", [this](){ copy(); });   // Todo: Use std::bind?
        menu->addSeparator();
        menu->addAction("Export...", [this](){ exportDialog(); });   // Todo: Use std::bind?
        menu->exec(plot->mapToGlobal(pos));
    });

    auto shortcut_copy = new QShortcut(QKeySequence::Copy, plot);
    QObject::connect(shortcut_copy, &QShortcut::activated, this, &Plot::copy);
}

void Plot::addSeries()
{
    auto curve = new QCPCurve(x_axis, y_axis);
    curve->setScatterSkip(0);
    series.push_back(curve);
}

void Plot::addSeries(const Series& data)
{
    addSeries();
    setData(series.size()-1, data);
}

void Plot::setData(size_t i, const Series& data)
{
    auto x = QVector<double>::fromStdVector(data.args());
    auto y = QVector<double>::fromStdVector(data.vals());
    series[i]->setData(x, y);
}

void Plot::setLineStyle(size_t i, QCPCurve::LineStyle style)
{
    series[i]->setLineStyle(style);
}

void Plot::setScatterStyle(size_t i, QCPScatterStyle style)
{
    series[i]->setScatterStyle(style);
}

void Plot::replot()
{
    plot->replot();
}

void Plot::includeOrigin(bool x, bool y)
{
    include_origin_x = x;
    include_origin_y = y;
}

void Plot::fixAspectRatio(bool value)
{
    fix_aspect_ratio = value;
}

void Plot::resizeEvent(QResizeEvent *event)
{
    plot->replot();
}

void Plot::copy()
{
    auto selection = plot->selectedPlottables();

    if(selection.size()  ==  0)
    {
        // Copy whole plot as pixmap
        QApplication::clipboard()->setPixmap(plot->toPixmap());
    }
    else
    {
        // Copy selected data series
        auto data = dynamic_cast<QCPCurve*>(selection[0])->data();
        QString result;
        for(auto it = data->begin(); it != data->end(); ++it)
        {
            result += QString::number((*it).key) + "\t" + QString::number((*it).value) + "\n";
        }

        QApplication::clipboard()->setText(result);
    }
}

void Plot::exportDialog()
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

