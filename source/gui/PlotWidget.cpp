#include "PlotWidget.hpp"

BackgroundDialog::BackgroundDialog(QWidget* parent, PlotWidget* plot)
    : BaseDialog(parent),
      plot(plot)
{
    label_extent_x = new QLabel("Width");
    label_extent_x->setAlignment(Qt::AlignRight);

    label_extent_y = new QLabel("Height");
    label_extent_y->setAlignment(Qt::AlignRight);

    label_offset_x = new QLabel("Offset X");
    label_offset_x->setAlignment(Qt::AlignRight);

    label_offset_y = new QLabel("Offset Y");
    label_offset_y->setAlignment(Qt::AlignRight);

    label_angle = new QLabel("Rotation");
    label_angle->setAlignment(Qt::AlignRight);

    spinner_extent_x = new QDoubleSpinBox();
    spinner_extent_x->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());

    spinner_extent_y = new QDoubleSpinBox();
    spinner_extent_y->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());

    spinner_offset_x = new QDoubleSpinBox();
    spinner_offset_x->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());

    spinner_offset_y = new QDoubleSpinBox();
    spinner_offset_y->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());

    spinner_angle = new QDoubleSpinBox();
    spinner_angle->setRange(-180.0, 180.0);

    cb_aspect_ratio = new QCheckBox("Keep aspect ratio");

    QObject::connect(spinner_extent_x, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &BackgroundDialog::modified);
    QObject::connect(spinner_extent_y, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &BackgroundDialog::modified);
    QObject::connect(spinner_offset_x, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &BackgroundDialog::modified);
    QObject::connect(spinner_offset_y, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &BackgroundDialog::modified);
    QObject::connect(spinner_angle, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &BackgroundDialog::modified);

    auto label_image = new QLabel("Image");
    label_image->setAlignment(Qt::AlignRight);

    auto edit_image = new QLineEdit();
    edit_image->setReadOnly(true);
    edit_image->setClearButtonEnabled(true);
    edit_image->setMinimumWidth(250);
    edit_image->setPlaceholderText("No file selected");

    // https://stackoverflow.com/a/57366109
    QToolButton* button_clear = edit_image->findChild<QToolButton*>();
    if(button_clear != nullptr) {
        button_clear->setEnabled(true);
        QObject::connect(button_clear, &QToolButton::clicked, [&]{
            setEditingEnabled(false);
            pixmap = QPixmap();
            emit modified();
        });
    }

    auto bt_open = new QPushButton(QIcon(":/icons/folder-black.svg"), "");
    QObject::connect(bt_open, &QPushButton::clicked, [this, edit_image]{
        QFileDialog dialog(this);
        dialog.setAcceptMode(QFileDialog::AcceptOpen);
        dialog.setFileMode(QFileDialog::ExistingFile);

        dialog.setNameFilters({
            "All Supported Files (*.bmp *.jpg *.jpeg *.png)",
            "BMP image (*.bmp)",
            "JPEG image (*.jpg *.jpeg)",
            "PNG image (*.png)",
            "SVG image (*.svg)"
        });

        if(dialog.exec() == QDialog::Accepted) {
            QStringList files = dialog.selectedFiles();
            if(files.size() == 1) {
                QFileInfo info(files[0]);
                edit_image->setText(info.fileName());

                // Load image and change opacity
                // https://www.qtcentre.org/threads/51158-setting-QPixmap-s-alpha-channel

                QPixmap input(files[0]);
                pixmap = QPixmap(input.size());
                pixmap.fill(Qt::transparent);

                QPainter painter(&pixmap);
                painter.setOpacity(0.5);    // Magic number
                painter.drawPixmap(0, 0, input);
                painter.end();

                setEditingEnabled(true);
                emit modified();
            }
        }
    });

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Reset);
    QObject::connect(buttons, &QDialogButtonBox::accepted, this, &BackgroundDialog::accept);

    auto hbox = new QHBoxLayout();
    hbox->setContentsMargins(0, 10, 0, 10);
    hbox->addWidget(edit_image);
    hbox->addWidget(bt_open);

    auto grid = new QGridLayout();
    grid->setSpacing(10);
    grid->setColumnStretch(0, 0);
    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(2, 0);
    grid->setColumnStretch(3, 1);

    grid->addWidget(label_image, 0, 0);
    grid->addLayout(hbox, 0, 1, 1, 3);

    grid->addWidget(label_extent_x, 1, 0);
    grid->addWidget(spinner_extent_x, 1, 1);

    grid->addWidget(label_offset_x, 1, 2);
    grid->addWidget(spinner_offset_x, 1, 3);

    grid->addWidget(label_extent_y, 2, 0);
    grid->addWidget(spinner_extent_y, 2, 1);

    grid->addWidget(label_offset_y, 2, 2);
    grid->addWidget(spinner_offset_y, 2, 3);

    grid->addWidget(cb_aspect_ratio, 3, 1);

    grid->addWidget(label_angle, 3, 2);
    grid->addWidget(spinner_angle, 3, 3);

    auto group = new QGroupBox();
    group->setLayout(grid);

    auto vbox = new QVBoxLayout();
    vbox->addWidget(group);
    vbox->addWidget(buttons);

    this->setWindowTitle("Background Image");
    this->setLayout(vbox);

    setEditingEnabled(false);
    resetTransform();
}

const QPixmap& BackgroundDialog::getPixmap() const {
    return pixmap;
}


double BackgroundDialog::getExtentX() const {
    return spinner_extent_x->value();
}

double BackgroundDialog::getExtentY() const {
    return spinner_extent_y->value();
}

double BackgroundDialog::getOffsetX() const {
    return spinner_offset_x->value();
}

double BackgroundDialog::getOffsetY() const {
    return spinner_offset_y->value();
}

double BackgroundDialog::getAngle() const {
    return spinner_angle->value();
}

void BackgroundDialog::setEditingEnabled(bool enabled) {
    spinner_extent_x->setEnabled(enabled);
    label_extent_x->setEnabled(enabled);

    spinner_extent_y->setEnabled(enabled);
    label_extent_y->setEnabled(enabled);

    spinner_offset_x->setEnabled(enabled);
    label_offset_x->setEnabled(enabled);

    spinner_offset_y->setEnabled(enabled);
    label_offset_y->setEnabled(enabled);

    spinner_angle->setEnabled(enabled);
    label_angle->setEnabled(enabled);

    cb_aspect_ratio->setEnabled(enabled);
}

void BackgroundDialog::resetTransform() {
    QCPRange range_x = plot->xAxis->range();
    QCPRange range_y = plot->yAxis->range();

    double extent_x = range_x.upper - range_x.lower;
    double extent_y = range_y.upper - range_y.lower;
    double offset_x = range_x.lower + 0.5*extent_x;
    double offset_y = range_y.lower + 0.5*extent_y;

    spinner_extent_x->setValue(extent_x);
    spinner_extent_y->setValue(extent_y);
    spinner_offset_x->setValue(offset_x);
    spinner_offset_y->setValue(offset_y);
    spinner_angle->setValue(0.0);
}

PlotWidget::PlotWidget()
    : menu(new QMenu(this)),
      aspect_policy(NONE),
      background_dialog(nullptr),
      background(new QCPItemPixmap(this))
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

    auto action_background_image = new QAction("Background image...", this);
    QObject::connect(action_background_image, &QAction::triggered, [&]{
        if(background_dialog == nullptr) {
            background_dialog = new BackgroundDialog(this, this);
            QObject::connect(background_dialog, &BackgroundDialog::modified, [&]{

                QPixmap pixmap = background_dialog->getPixmap();
                if(pixmap.isNull()) {
                    background->setVisible(false);
                    return;
                }

                double x_extent = background_dialog->getExtentX();
                double y_extent = background_dialog->getExtentY();

                double x_offset = background_dialog->getOffsetX();;
                double y_offset = background_dialog->getOffsetY();

                double angle = background_dialog->getAngle();

                QTransform translation = QTransform().translate(x_offset, y_offset);
                QTransform rotation = QTransform().rotate(-angle);
                QTransform transform = rotation*translation;

                QPointF p0 = QPointF( 0.5*x_extent,  0.5*y_extent);
                QPointF p1 = QPointF( 0.5*x_extent, -0.5*y_extent);
                QPointF p2 = QPointF(-0.5*x_extent,  0.5*y_extent);
                QPointF p3 = QPointF(-0.5*x_extent, -0.5*y_extent);

                p0 = transform*p0;
                p1 = transform*p1;
                p2 = transform*p2;
                p3 = transform*p3;

                double x_min = std::min({p0.x(), p1.x(), p2.x(), p3.x()});
                double x_max = std::max({p0.x(), p1.x(), p2.x(), p3.x()});

                double y_min = std::min({p0.y(), p1.y(), p2.y(), p3.y()});
                double y_max = std::max({p0.y(), p1.y(), p2.y(), p3.y()});

                qInfo() << x_min << ", " << x_max << ", " << y_min << ", " << y_max;

                background->setPixmap(pixmap.transformed(rotation, Qt::SmoothTransformation));
                background->setScaled(true, Qt::IgnoreAspectRatio);
                background->setVisible(true);

                background->topLeft->setType(QCPItemPosition::ptPlotCoords);
                background->topLeft->setAxes(this->xAxis, this->yAxis);
                background->topLeft->setCoords(x_min, y_max);

                background->bottomRight->setType(QCPItemPosition::ptPlotCoords);
                background->bottomRight->setAxes(this->xAxis, this->yAxis);
                background->bottomRight->setCoords(x_max, y_min);

                this->replot();


            });


        }

        background_dialog->show();
    });
    menu->addAction(action_background_image);

    auto action_export = new QAction("Export as...", this);
    QObject::connect(action_export, &QAction::triggered, this, &PlotWidget::onExport);
    menu->addAction(action_export);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(this, &QCustomPlot::customContextMenuRequested, [this](QPoint pos) {
        menu->exec(this->mapToGlobal(pos));
    });

    // Limit axis ranges for zooming and panning

    QObject::connect(this->xAxis, static_cast<void (QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged), [&](const QCPRange& range) {
        if(max_x_range) {
            QCPRange bounded = range.bounded(max_x_range->lower, max_x_range->upper);
            this->xAxis->setRange(bounded);
        }
    });

    QObject::connect(this->yAxis, static_cast<void (QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged), [&](const QCPRange& range) {
        if(max_y_range) {
            QCPRange bounded = range.bounded(max_y_range->lower, max_y_range->upper);
            this->yAxis->setRange(bounded);
        }
    });

    // Handle aspect ratio

    QObject::connect(this, &QCustomPlot::beforeReplot, [&] {
        switch(aspect_policy) {
            case SCALE_X: {
                const QSignalBlocker blocker(this->xAxis);
                this->xAxis->setScaleRatio(this->yAxis);
                break;
            }
            case SCALE_Y: {
                const QSignalBlocker blocker(this->yAxis);
                this->yAxis->setScaleRatio(this->xAxis);
                break;
            }
            case NONE: {
                break;
            }
        }
    });
}

QMenu* PlotWidget::contextMenu() {
    return menu;
}

void PlotWidget::setupTopLegend() {
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
void PlotWidget::rescaleAxes(bool include_zero_x, bool include_zero_y) {
    max_x_range = std::nullopt;
    max_y_range = std::nullopt;

    QCustomPlot::rescaleAxes();

    QCPRange x_range = xAxis->range();
    QCPRange y_range = yAxis->range();

    if(include_zero_x) {
        x_range.expand(0.0);
    }
    if(include_zero_y) {
        y_range.expand(0.0);
    }
    setAxesLimits(x_range, y_range);
}

void PlotWidget::setAxesLimits(QCPRange x_range, QCPRange y_range) {
    max_x_range = x_range;
    max_y_range = y_range;

    xAxis->setRange(x_range);
    yAxis->setRange(y_range);
}

void PlotWidget::setAspectPolicy(AspectPolicy policy) {
    aspect_policy = policy;
}

QSize PlotWidget::sizeHint() const {
    return {600, 400};
}

void PlotWidget::resizeEvent(QResizeEvent * event) {
    QCustomPlot::resizeEvent(event);

    // Reflow legend

    int legend_width = this->legend->rect().width();
    int items_width = 0;

    int item_index;
    for(item_index = 0; item_index < this->legend->itemCount(); ++item_index) {
        auto item = this->legend->item(item_index);
        if(item == nullptr) {
            break;
        }

        items_width += item->minimumSizeHint().width()
                + this->legend->margins().left()
                + this->legend->margins().right();

        if(items_width > legend_width) {
            break;
        }
    }

    this->legend->setWrap(item_index);
    this->legend->setFillOrder(QCPLayoutGrid::foColumnsFirst, true);

    // Axis scaling

    if(aspect_policy != NONE) {
        this->replot();
    }
}

void PlotWidget::onExport() {
    const char* PDF_FILE  = "Portable Document Format (*.pdf)";
    const char* PNG_FILE  = "PNG image (*.png)";
    const char* BMP_FILE  = "BMP image (*.bmp)";

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilters({PDF_FILE, PNG_FILE, BMP_FILE});

    // Todo: Is there a better way to connect default suffix to the selected name filter?
    // Todo: Handle the case of the save[...] methods returning false
    QObject::connect(&dialog, &QFileDialog::filterSelected, [&](const QString &filter) {
        if(filter == PDF_FILE) {
            dialog.setDefaultSuffix(".pdf");
        }
        else if(filter == PNG_FILE) {
            dialog.setDefaultSuffix(".png");
        }
        else if(filter == BMP_FILE) {
            dialog.setDefaultSuffix(".bmp");
        }
    });
    dialog.filterSelected(PNG_FILE);

    if(dialog.exec() == QDialog::Accepted) {
        QString filter = dialog.selectedNameFilter();
        QString path = dialog.selectedFiles().first();

        const double scale = 2.0;    // Magic number

        if(filter == PDF_FILE) {
            this->savePdf(path);
        }
        else if(filter == PNG_FILE) {
            this->savePng(path, 0, 0, scale);
        }
        else if(filter == BMP_FILE) {
            this->saveBmp(path, 0, 0, scale);
        }
    }
}
