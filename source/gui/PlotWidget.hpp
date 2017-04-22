#pragma once
#include "external/qcustomplot/qcustomplot.h"

class PlotWidget: public QCustomPlot
{
public:
    PlotWidget(const QSize& size_hint = {600, 400})
        : size_hint(size_hint)
    {    
        // Styling

        this->xAxis2->setTickLabels(false);
        this->xAxis2->setVisible(true);
        this->yAxis2->setTickLabels(false);
        this->yAxis2->setVisible(true);

        // Interaction

        this->setInteraction(QCP::iRangeDrag, true);
        this->setInteraction(QCP::iRangeZoom, true);

        // Context menu

        this->setContextMenuPolicy(Qt::CustomContextMenu);
        QObject::connect(this, &QCustomPlot::customContextMenuRequested, [&](QPoint pos)
        {
            auto menu = new QMenu(this);
            menu->addAction("Export as...", this, [&]()
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
    }

    void setupTopLegend()
    {
        this->legend->setVisible(true);
        this->legend->setFillOrder(QCPLayoutGrid::foColumnsFirst);

        // Move legend at to the top outside of the plot
        // http://qcustomplot.com/index.php/support/forum/63
        // Todo: Better solution with QCustomPlot 2.0?
        QCPLayoutGrid *subLayout = new QCPLayoutGrid;
        this->plotLayout()->insertRow(0);
        this->plotLayout()->addElement(0, 0, subLayout);
        subLayout->addElement(0, 0, new QCPLayoutElement);
        subLayout->addElement(0, 1, this->legend);
        subLayout->addElement(0, 2, new QCPLayoutElement);
        this->plotLayout()->setRowStretchFactor(0, 0.001);
    }

    virtual QSize sizeHint() const
    {
        return size_hint;
    }

private:
    QSize size_hint;
};
