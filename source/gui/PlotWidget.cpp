#include "PlotWidget.hpp"

PlotWidget::PlotWidget(const QSize& size_hint)
    : size_hint(size_hint),
      aspect_policy(NONE)
{
    // Styling

    this->xAxis2->setTicks(false);
    this->yAxis2->setTicks(false);
    this->xAxis2->setVisible(true);
    this->yAxis2->setVisible(true);

    auto ticker = QSharedPointer<QCPAxisTicker>(new QCPAxisTicker());
    ticker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
    ticker->setTickCount(6);

    this->xAxis->setTicker(ticker);
    this->yAxis->setTicker(ticker);

    // Interaction

    this->setInteraction(QCP::iRangeDrag, true);
    this->setInteraction(QCP::iRangeZoom, true);

    // Context menu

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(this, &QCustomPlot::customContextMenuRequested, [&](QPoint pos)
    {
        auto menu = new QMenu(this);
        QObject::connect(menu->addAction("Export as..."), &QAction::triggered, [&]
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
            // Todo: Handle the case of the save[...] methods returning false
            QObject::connect(&dialog, &QFileDialog::filterSelected, [&](const QString &filter)
            {
                if(filter == PNG_FILE)
                    dialog.setDefaultSuffix(".png");
                else if(filter == BMP_FILE)
                    dialog.setDefaultSuffix(".bmp");
                else if(filter == PDF_FILE)
                    dialog.setDefaultSuffix(".pdf");
            });
            dialog.filterSelected(PNG_FILE);

            if(dialog.exec() == QDialog::Accepted)
            {
                QString filter = dialog.selectedNameFilter();
                QString path = dialog.selectedFiles().first();

                if(filter == PNG_FILE)
                    this->savePng(path);
                else if(filter == BMP_FILE)
                    this->saveBmp(path);
                else if(filter == PDF_FILE)
                    this->savePdf(path);
            }
        });

        menu->exec(this->mapToGlobal(pos));
    });

    // Limit axis ranges for zooming and panning

    QObject::connect(this->xAxis, static_cast<void (QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged), [&](const QCPRange& range)
    {
        if(max_x_range)
        {
            QCPRange bounded = range.bounded(max_x_range->lower, max_x_range->upper);
            this->xAxis->setRange(bounded);
        }
    });

    QObject::connect(this->yAxis, static_cast<void (QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged), [&](const QCPRange& range)
    {
        if(max_y_range)
        {
            QCPRange bounded = range.bounded(max_y_range->lower, max_y_range->upper);
            this->yAxis->setRange(bounded);
        }
    });

    // Handle aspect ratio

    QObject::connect(this, &QCustomPlot::beforeReplot, [&]
    {
        switch(aspect_policy)
        {
        case SCALE_X:
            this->xAxis->blockSignals(true);
            this->xAxis->setScaleRatio(this->yAxis);
            this->xAxis->blockSignals(false);
            break;

        case SCALE_Y:
            this->yAxis->blockSignals(true);
            this->yAxis->setScaleRatio(this->xAxis);
            this->yAxis->blockSignals(false);
            break;
        }
    });

}

void PlotWidget::setupTopLegend()
{
    // Legend properties
    this->legend->setVisible(true);
    this->legend->setFillOrder(QCPLayoutGrid::foColumnsFirst);

    // Create sublayout and add legend to it
    QCPLayoutGrid *sub_layout = new QCPLayoutGrid();
    sub_layout->setMargins({0, 11, 0, 0});                // Magic number
    sub_layout->addElement(0, 0, this->legend);

    // Create a new row in the plot layout and add sublayout
    this->plotLayout()->insertRow(0);
    this->plotLayout()->setRowStretchFactor(0, 0.001);    // Magic number
    this->plotLayout()->setRowSpacing(0);
    this->plotLayout()->addElement(0, 0, sub_layout);

    // Connect horizontal margins of the sublayout those of the axis rectangle
    auto group = new QCPMarginGroup(this);
    sub_layout->setMarginGroup(QCP::msLeft|QCP::msRight, group);
    this->axisRect(0)->setMarginGroup(QCP::msLeft|QCP::msRight, group);
}

// Limit the axis maximum ranges to current range
void PlotWidget::rescaleAxes(bool include_zero_x, bool include_zero_y)
{
    max_x_range = boost::none;
    max_y_range = boost::none;

    QCustomPlot::rescaleAxes();

    QCPRange x_range = xAxis->range();
    QCPRange y_range = yAxis->range();

    if(include_zero_x)
        x_range.expand(0.0);

    if(include_zero_y)
        y_range.expand(0.0);

    setAxesLimits(x_range, y_range);
}

void PlotWidget::setAxesLimits(QCPRange x_range, QCPRange y_range)
{
    max_x_range = x_range;
    max_y_range = y_range;

    xAxis->setRange(x_range);
    yAxis->setRange(y_range);
}

void PlotWidget::setAspectPolicy(AspectPolicy policy)
{
    aspect_policy = policy;
}

QSize PlotWidget::sizeHint() const
{
    return size_hint;
}

void PlotWidget::resizeEvent(QResizeEvent * event)
{
    QCustomPlot::resizeEvent(event);

    // Reflow legend

    int legend_width = this->legend->rect().width();
    int items_width = 0;

    int item_index;
    for(item_index = 0; item_index < this->legend->itemCount(); ++item_index)
    {
        auto item = this->legend->item(item_index);
        if(item == nullptr)
            break;

        items_width += item->minimumSizeHint().width()
                + this->legend->margins().left()
                + this->legend->margins().right();

        if(items_width > legend_width)
            break;
    }

    this->legend->setWrap(item_index);
    this->legend->setFillOrder(QCPLayoutGrid::foColumnsFirst, true);

    // Axis scaling

    if(aspect_policy != NONE)
        this->replot();
}
