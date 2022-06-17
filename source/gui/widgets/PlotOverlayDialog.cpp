#include "PlotOverlayDialog.hpp"
#include "gui/widgets/PlotWidget.hpp"

// Magic numbers
const int N_STEPS_TRANSLATION = 250;
const double STEP_SIZE_ROTATION = 0.25;
const double BACKGROUND_OPACITY = 0.5;

PlotOverlayDialog::PlotOverlayDialog(PlotWidget* plot)
    : DialogBase(plot),
      plot(plot)
{
    item = new QCPItemPixmap(plot);
    item->setScaled(true, Qt::IgnoreAspectRatio);
    item->setLayer("background");

    QLabel* label_extent_x = new QLabel("Width");
    label_extent_x->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QLabel* label_extent_y = new QLabel("Height");
    label_extent_y->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QLabel* label_offset_x = new QLabel("Offset X");
    label_offset_x->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QLabel* label_offset_y = new QLabel("Offset Y");
    label_offset_y->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QLabel* label_angle = new QLabel("Rotation");
    label_angle->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    spinner_extent_x = new QDoubleSpinBox();
    spinner_extent_x->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    QObject::connect(spinner_extent_x, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](){
        updateAspectRatioY();
        updateItem();
    });

    spinner_extent_y = new QDoubleSpinBox();
    spinner_extent_y->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    QObject::connect(spinner_extent_y, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](){
        updateAspectRatioX();
        updateItem();
    });

    cb_aspect_ratio = new QCheckBox("Keep aspect ratio");
    QObject::connect(cb_aspect_ratio, &QCheckBox::stateChanged, [&](int state){
        if(state == Qt::Checked) {
            updateAspectRatioY();
            updateItem();
        }
    });

    spinner_offset_x = new QDoubleSpinBox();
    spinner_offset_x->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    QObject::connect(spinner_offset_x, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &PlotOverlayDialog::updateItem);

    spinner_offset_y = new QDoubleSpinBox();
    spinner_offset_y->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    QObject::connect(spinner_offset_y, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &PlotOverlayDialog::updateItem);

    spinner_angle = new QDoubleSpinBox();
    spinner_angle->setRange(-180.0, 180.0);
    spinner_angle->setSingleStep(STEP_SIZE_ROTATION);
    spinner_angle->setSuffix("°");
    QObject::connect(spinner_angle, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &PlotOverlayDialog::updateItem);

    auto edit_image = new QLineEdit();
    edit_image->setPlaceholderText("No file selected");
    edit_image->setReadOnly(true);
    edit_image->setClearButtonEnabled(true);
    edit_image->setMinimumWidth(250);

    // Enable clear button despite the LineEdit being read-only, https://stackoverflow.com/a/57366109
    auto button_clear = edit_image->findChild<QToolButton*>();
    if(button_clear != nullptr) {
        button_clear->setEnabled(true);
        QObject::connect(button_clear, &QToolButton::clicked, [&]{
            updateImage();
        });
    }

    auto button_open = new QToolButton();
    button_open->setIcon(QIcon(":/icons/folder-black.svg"));
    button_open->setFixedHeight(edit_image->sizeHint().height());
    QObject::connect(button_open, &QToolButton::clicked, [this, edit_image]{
        QFileDialog dialog(this);
        dialog.setAcceptMode(QFileDialog::AcceptOpen);
        dialog.setFileMode(QFileDialog::ExistingFile);
        dialog.setNameFilters({"Image Files (*.bmp *.jpg *.jpeg *.png *.svg)"});

        if(dialog.exec() == QDialog::Accepted) {
            QStringList files = dialog.selectedFiles();
            if(files.size() == 1) {
                QFileInfo info(files[0]);
                edit_image->setText(info.fileName());
                updateImage(files[0]);
            }
        }
    });

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Reset);
    QObject::connect(buttons, &QDialogButtonBox::clicked, [&, buttons](QAbstractButton* button){
        switch(buttons->standardButton(button)) {
        case QDialogButtonBox::Ok:
            accept();
            break;

        case QDialogButtonBox::Reset:
            resetTransform();
            break;

        default:
            break;
        }
    });

    auto hbox = new QHBoxLayout();
    hbox->setSpacing(10);
    hbox->addWidget(edit_image);
    hbox->addWidget(button_open);

    auto grid = new QGridLayout();
    grid->setSpacing(10);
    grid->setColumnStretch(0, 0);
    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(2, 0);
    grid->setColumnStretch(3, 1);

    grid->addWidget(label_extent_x, 0, 0);
    grid->addWidget(spinner_extent_x, 0, 1);
    grid->addWidget(label_offset_x, 0, 2);
    grid->addWidget(spinner_offset_x, 0, 3);

    grid->addWidget(label_extent_y, 1, 0);
    grid->addWidget(spinner_extent_y, 1, 1);
    grid->addWidget(label_offset_y, 1, 2);
    grid->addWidget(spinner_offset_y, 1, 3);

    grid->addWidget(cb_aspect_ratio, 2, 1);
    grid->addWidget(label_angle, 2, 2);
    grid->addWidget(spinner_angle, 2, 3);

    group_image = new QGroupBox("Image");
    group_image->setLayout(hbox);

    group_trans = new QGroupBox("Transform");
    group_trans->setLayout(grid);

    auto vbox = new QVBoxLayout();
    vbox->setSizeConstraint(QLayout::SetFixedSize);
    vbox->addWidget(group_image);
    vbox->addWidget(group_trans);
    vbox->addWidget(buttons);

    this->setWindowTitle("Background Image");
    this->setLayout(vbox);

    setEditingEnabled(false);
    resetTransform();
}

void PlotOverlayDialog::setEditingEnabled(bool enabled) {
    group_trans->setEnabled(enabled);
}

// Reset transformation such that the image fits with the plot axes
void PlotOverlayDialog::resetTransform() {
    QCPRange range_x = plot->xAxis->range();
    QCPRange range_y = plot->yAxis->range();

    double extent_x = range_x.upper - range_x.lower;
    double extent_y = range_y.upper - range_y.lower;
    double offset_x = range_x.lower + 0.5*extent_x;
    double offset_y = range_y.lower + 0.5*extent_y;

    double delta_x = extent_x/N_STEPS_TRANSLATION;
    double delta_y = extent_y/N_STEPS_TRANSLATION;

    cb_aspect_ratio->setChecked(false);

    {
        QSignalBlocker blocker(spinner_extent_x);
        spinner_extent_x->setValue(extent_x);
        spinner_extent_x->setSingleStep(delta_x);
    }
    {
        QSignalBlocker blocker(spinner_extent_y);
        spinner_extent_y->setValue(extent_y);
        spinner_extent_y->setSingleStep(delta_y);
    }
    {
        QSignalBlocker blocker(spinner_offset_x);
        spinner_offset_x->setValue(offset_x);
        spinner_offset_x->setSingleStep(delta_x);
    }
    {
        QSignalBlocker blocker(spinner_offset_y);
        spinner_offset_y->setValue(offset_y);
        spinner_offset_y->setSingleStep(delta_y);
    }
    {
        QSignalBlocker blocker(spinner_angle);
        spinner_angle->setValue(0.0);
    }

    updateItem();
}

// Change x extent to match aspect ratio of the image
void PlotOverlayDialog::updateAspectRatioX() {
    if(cb_aspect_ratio->isChecked() && pixmap.height() != 0) {
        QSignalBlocker blocker(spinner_extent_x);
        spinner_extent_x->setValue(spinner_extent_y->value()*pixmap.width()/pixmap.height());
    }
}

// Change y extent to match aspect ratio of the image
void PlotOverlayDialog::updateAspectRatioY() {
    if(cb_aspect_ratio->isChecked() && pixmap.width() != 0) {
        QSignalBlocker blocker(spinner_extent_y);
        spinner_extent_y->setValue(spinner_extent_x->value()*pixmap.height()/pixmap.width());
    }
}

// Update the pixmap either with an empty one or by loading from a file
void PlotOverlayDialog::updateImage(const QString& file) {
    if(file.isEmpty()) {
        pixmap = QPixmap();
        setEditingEnabled(false);
    }
    else {
        QPixmap input(file);
        pixmap = QPixmap(input.size());
        pixmap.fill(Qt::transparent);

        // Change opacity
        // https://www.qtcentre.org/threads/51158-setting-QPixmap-s-alpha-channel
        QPainter painter(&pixmap);
        painter.setOpacity(BACKGROUND_OPACITY);
        painter.drawPixmap(0, 0, input);
        painter.end();

        updateAspectRatioY();
        setEditingEnabled(true);
    }

    updateItem();
}

// Update the plot item with the current pixmap and transformation
void PlotOverlayDialog::updateItem() {
    if(pixmap.isNull()) {
        item->setVisible(false);
        plot->replot();
        return;
    }

    double x_extent = spinner_extent_x->value();    // Width of the picture in plot units
    double y_extent = spinner_extent_y->value();    // Height of the picture in plot units
    double x_offset = spinner_offset_x->value();    // X coordinate of the image center in plot units
    double y_offset = spinner_offset_y->value();    // Y coordinate of the image center in plot units
    double angle = spinner_angle->value();          // Rotation of the image around its center

    QTransform translation = QTransform().translate(x_offset, y_offset);
    QTransform rotation = QTransform().rotate(-angle);
    QTransform transform = rotation*translation;

    QPointF p0 = transform*QPointF( 0.5*x_extent,  0.5*y_extent);
    QPointF p1 = transform*QPointF( 0.5*x_extent, -0.5*y_extent);
    QPointF p2 = transform*QPointF(-0.5*x_extent,  0.5*y_extent);
    QPointF p3 = transform*QPointF(-0.5*x_extent, -0.5*y_extent);

    double x_min = std::min({p0.x(), p1.x(), p2.x(), p3.x()});
    double x_max = std::max({p0.x(), p1.x(), p2.x(), p3.x()});
    double y_min = std::min({p0.y(), p1.y(), p2.y(), p3.y()});
    double y_max = std::max({p0.y(), p1.y(), p2.y(), p3.y()});

    item->topLeft->setType(QCPItemPosition::ptPlotCoords);
    item->topLeft->setAxes(plot->xAxis, plot->yAxis);
    item->topLeft->setCoords(x_min, y_max);

    item->bottomRight->setType(QCPItemPosition::ptPlotCoords);
    item->bottomRight->setAxes(plot->xAxis, plot->yAxis);
    item->bottomRight->setCoords(x_max, y_min);

    item->setPixmap(pixmap.transformed(rotation, Qt::SmoothTransformation));
    item->setVisible(true);

    plot->replot();
}
