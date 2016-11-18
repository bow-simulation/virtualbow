#include "Plot.hpp"

Style Style::Line(QColor color, int width)
{
    Style style;
    style.line_style = QCPCurve::lsLine;
    style.line_pen = QPen(QBrush(color), width);
    return style;
}

Style Style::Brush(QColor color)
{
    Style style;
    style.line_brush = QBrush(color);
    return style;
}

Style Style::Scatter(QCPScatterStyle::ScatterShape shape, QColor color, int size)
{
    Style style;
    style.scatter_style = QCPScatterStyle(shape, color, size);
    return style;
}

Plot::Plot(const QString& lbx, const QString& lby, Align align)
    : plot(new QCustomPlot),
      content_range_x(0.0, 1.0),
      content_range_y(0.0, 1.0),
      mode_x(ExpansionMode::None),
      mode_y(ExpansionMode::None)
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
        auto expand_axis = [](QCPAxis* axis, double size, ExpansionMode mode)
        {
            switch(mode)
            {
            case ExpansionMode::None:
                break;

            case ExpansionMode::OneSided:
                axis->setRangeUpper(axis->range().lower + size);
                break;

            case ExpansionMode::Symmetric:
                double center = axis->range().center();
                axis->setRangeLower(center - 0.5*size);
                axis->setRangeUpper(center + 0.5*size);
                break;
            }
        };

        double w = plot->axisRect(0)->width();
        double h = plot->axisRect(0)->height();
        double size_x = content_range_x.size();
        double size_y = content_range_y.size();

        x_axis->setRange(content_range_x);
        y_axis->setRange(content_range_y);

        if(h/w > size_y/size_x)
            expand_axis(y_axis, h/w*size_x, mode_y);
        else
            expand_axis(x_axis, w/h*size_y, mode_x);

        // Adjust axes such that the last label is drawn
        // Todo: Not a complete solution, only works for labels which have a "nice" value
        double epsilon = std::numeric_limits<double>::epsilon();
        x_axis->setRangeLower(x_axis->range().lower - epsilon);
        x_axis->setRangeUpper(x_axis->range().upper + epsilon);
        y_axis->setRangeLower(y_axis->range().lower - epsilon);
        y_axis->setRangeUpper(y_axis->range().upper + epsilon);
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

size_t Plot::addSeries(const Series& data, const Style& style, const QString& name)
{
    auto curve = new QCPCurve(x_axis, y_axis);
    curve->setScatterSkip(0);
    series.push_back(curve);

    size_t index = series.size() - 1;
    setData(index, data);
    setStyle(index, style);
    setName(index, name);

    return index;
}

void Plot::setData(size_t i, const Series& data)
{
    auto x = QVector<double>::fromStdVector(data.args());
    auto y = QVector<double>::fromStdVector(data.vals());
    series[i]->setData(x, y);
}

void Plot::setStyle(size_t i, const Style& style)
{
    series[i]->setLineStyle(style.line_style);
    series[i]->setPen(style.line_pen);
    series[i]->setBrush(style.line_brush);
    series[i]->setScatterStyle(style.scatter_style);
}

void Plot::setName(size_t i, const QString& name)
{
    series[i]->setName(name);
}

void Plot::setExpansionMode(ExpansionMode em_x, ExpansionMode em_y)
{
    mode_x = em_x;
    mode_y = em_y;
}

void Plot::fitContent(bool include_origin_x, bool include_origin_y)
{
    content_range_x = QCPRange();
    content_range_y = QCPRange();

    for(auto s: series)
    {
        bool exists_x;
        bool exists_y;
        QCPRange range_x = s->getKeyRange(exists_x);
        QCPRange range_y = s->getValueRange(exists_y);

        if(exists_x)
            content_range_x.expand(range_x);
        if(exists_y)
            content_range_y.expand(range_y);
    }

    auto include_origin = [this](QCPAxis* axis)
    {
        auto range = axis->range();
        range.expand(0.0);
        axis->setRange(range);
    };

    if(include_origin_x)
        include_origin(x_axis);

    if(include_origin_y)
        include_origin(y_axis);
}

void Plot::setContentRanges(const QCPRange& rx, const QCPRange& ry)
{
    content_range_x = rx;
    content_range_y = ry;
}

void Plot::replot()
{
    plot->replot();
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

