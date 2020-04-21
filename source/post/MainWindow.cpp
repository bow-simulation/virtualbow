#include "MainWindow.hpp"
#include "NumberGrid.hpp"
#include "ShapePlot.hpp"
#include "StressPlot.hpp"
#include "CurvaturePlot.hpp"
#include "EnergyPlot.hpp"
#include "ComboPlot.hpp"
#include "Slider.hpp"
#include "config.hpp"
#include <algorithm>

MainWindow::MainWindow()
    : menu_recent(new RecentFilesMenu(this))
{
    // Actions
    auto action_open = new QAction(QIcon(":/icons/document-open.png"), "&Open...", this);
    action_open->setShortcuts(QKeySequence::Open);
    action_open->setMenuRole(QAction::NoRole);
    QObject::connect(action_open, &QAction::triggered, this, &MainWindow::open);

    auto action_save_as = new QAction(QIcon(":/icons/document-save-as.png"), "Save &As...", this);
    action_save_as->setShortcuts(QKeySequence::SaveAs);
    action_save_as->setMenuRole(QAction::NoRole);
    QObject::connect(action_save_as, &QAction::triggered, this, &MainWindow::saveAs);

    auto action_quit = new QAction(QIcon(":/icons/application-exit.png"), "&Quit", this);
    QObject::connect(action_quit, &QAction::triggered, this, &QWidget::close);
    action_quit->setShortcuts(QKeySequence::Quit);
    action_quit->setMenuRole(QAction::QuitRole);

    auto action_about = new QAction(QIcon(":/icons/dialog-information.png"), "&About...", this);
    QObject::connect(action_about, &QAction::triggered, this, &MainWindow::about);
    action_about->setMenuRole(QAction::AboutRole);
    action_about->setIconVisibleInMenu(true);

    // Recent file menu
    QObject::connect(menu_recent, &RecentFilesMenu::openRecent, this, &MainWindow::loadFile);

    // File menu
    auto menu_file = this->menuBar()->addMenu("&File");
    menu_file->addAction(action_open);
    menu_file->addMenu(menu_recent);
    menu_file->addSeparator();
    menu_file->addAction(action_save_as);
    menu_file->addSeparator();
    menu_file->addAction(action_quit);

    // Help menu
    auto menu_help = this->menuBar()->addMenu("&Help");
    menu_help->addAction(action_about);

    // Main window
    this->setWindowIcon(QIcon(":/icons/logo"));

    // Load geometry and state
    QSettings settings;
    restoreState(settings.value("OutputWindow/state").toByteArray());
    restoreGeometry(settings.value("OutputWindow/geometry").toByteArray());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Save state and geometry
    QSettings settings;
    settings.setValue("OutputWindow/state", saveState());
    settings.setValue("OutputWindow/geometry", saveGeometry());
}

void MainWindow::loadFile(const QString& path)
{
    try
    {
        OutputData output(path.toStdString());
        this->setCentralWidget(new OutputWidget(output));
        this->setWindowFilePath(path);
        menu_recent->addPath(path);
    }
    catch(const std::exception& e)  // Todo
    {
        QMessageBox::critical(this, "", "Failed to open " + path + "\n" + e.what());
    }
}

void MainWindow::saveFile(const QString &path)
{
    try
    {
        auto widget = static_cast<OutputWidget*>(this->centralWidget());
        widget->getData().save(path.toStdString());
        this->setWindowFilePath(path);
    }
    catch(const std::exception& e)  // Todo
    {
        QMessageBox::critical(this, "", "Failed to save " + path + "\n" + e.what());
    }
}

void MainWindow::open()
{
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter("Result Files (*.res)");

    if(dialog.exec() == QDialog::Accepted)
        loadFile(dialog.selectedFiles().first());
}

void MainWindow::saveAs()
{
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter("Result Files (*.red)");
    dialog.setDefaultSuffix("res");
    dialog.selectFile(this->windowFilePath().isEmpty() ? DEFAULT_FILENAME : this->windowFilePath());

    if(dialog.exec() == QDialog::Accepted)
        saveFile(dialog.selectedFiles().first());
}

void MainWindow::about()
{
    QMessageBox::about(this, "About", QString()
        + "<strong><font size=\"6\">" + Config::APPLICATION_DISPLAY_NAME_POST + "</font></strong><br>"
        + "Version " + Config::APPLICATION_VERSION + "<br><br>"
        + Config::APPLICATION_DESCRIPTION + "<br>"
        + "<a href=\"" + Config::ORGANIZATION_DOMAIN + "\">" + Config::ORGANIZATION_DOMAIN + "</a><br><br>"
        + "<small>" + Config::APPLICATION_COPYRIGHT + "<br>"
        + "Distributed under the " + Config::APPLICATION_LICENSE + "</small>"
    );
}














OutputWidget::OutputWidget(const OutputData& output)
    : data(output)
{
    bool enable_statics = !data.statics.states.time.empty();
    bool enable_dynamics = !data.dynamics.states.time.empty();

    auto stack = new QStackedLayout();
    if(enable_statics) {
        stack->addWidget(new StaticOutputWidget(data));
    }
    if(enable_dynamics) {
        stack->addWidget(new DynamicOutputWidget(data));
    }

    auto bt_statics = new QPushButton("Statics");
    // bt_statics->setStyleSheet("background-color: rgb(249, 217, 111);");
    bt_statics->setIcon(QIcon(":/icons/show-statics"));
    bt_statics->setCheckable(true);
    bt_statics->setEnabled(enable_statics);
    bt_statics->setAutoExclusive(true);
    QObject::connect(bt_statics, &QPushButton::toggled, [=](bool checked) {
        if(checked)
            stack->setCurrentIndex(0);
    });

    auto bt_dynamics = new QPushButton("Dynamics");
    // bt_dynamics->setStyleSheet("background-color: rgb(170, 243, 117);");
    bt_dynamics->setIcon(QIcon(":/icons/show-dynamics"));
    bt_dynamics->setCheckable(true);
    bt_dynamics->setEnabled(enable_dynamics);
    bt_dynamics->setAutoExclusive(true);
    QObject::connect(bt_dynamics, &QPushButton::toggled, [=](bool checked) {
        if(checked)
            stack->setCurrentIndex(1);
    });

    auto btbox = new QDialogButtonBox();
    btbox->addButton(bt_statics, QDialogButtonBox::ActionRole);
    btbox->addButton(bt_dynamics, QDialogButtonBox::ActionRole);
    QObject::connect(btbox, &QDialogButtonBox::rejected, this, &QDialog::close);

    auto vbox = new QVBoxLayout();
    vbox->addLayout(stack, 1);
    vbox->addWidget(btbox);
    this->setLayout(vbox);

    bt_statics->setChecked(!enable_dynamics);
    bt_dynamics->setChecked(enable_dynamics);
}

const OutputData& OutputWidget::getData()
{
    return data;
}


StaticOutputWidget::StaticOutputWidget(const OutputData& data)
{
    auto numbers = new NumberGrid();
    numbers->addColumn();
    numbers->addGroup("Performance");
    numbers->addValue("Final draw force [N]", data.statics.final_draw_force);
    numbers->addValue("Drawing work [J]", data.statics.drawing_work);
    numbers->addValue("Storage ratio", data.statics.storage_ratio);
    numbers->addGroup("Properties");
    numbers->addValue("Limb mass [kg]", data.setup.limb_mass);
    numbers->addValue("String mass [kg]", data.setup.string_mass);
    numbers->addValue("String length [m]", data.setup.string_length);
    numbers->addColumn();
    numbers->addGroup("Maximum absolute stresses");
    for(int i = 0; i < data.statics.max_stress_index.size(); ++i) {
        numbers->addValue(QString::fromStdString(data.setup.limb_properties.layers[i].name + " [Pa]"), data.statics.max_stress_value[i]);
    }
    numbers->addGroup("Maximum absolute forces");
    numbers->addValue("Draw force [N]", data.statics.states.draw_force[data.statics.max_draw_force_index]);
    numbers->addValue("Grip force [N]", data.statics.states.grip_force[data.statics.max_grip_force_index]);
    numbers->addValue("String force (total) [N]", data.statics.states.string_force[data.statics.max_string_force_index]);
    numbers->addValue("String force (strand) [N]", data.statics.states.strand_force[data.statics.max_string_force_index]);

    auto plot_shapes = new ShapePlot(data.setup.limb_properties, data.statics.states, true);
    auto plot_stress = new StressPlot(data.setup.limb_properties, data.statics.states);
    auto plot_curvature = new CurvaturePlot(data.setup.limb_properties, data.statics.states);
    auto plot_energy = new EnergyPlot(data.statics.states, data.statics.states.draw_length, "Draw length [m]");
    auto plot_combo = new ComboPlot();
    plot_combo->addData("Draw length [m]", data.statics.states.draw_length);
    plot_combo->addData("Draw force [N]", data.statics.states.draw_force);
    plot_combo->addData("String force (total) [N]", data.statics.states.string_force);
    plot_combo->addData("String force (strand) [N]", data.statics.states.strand_force);
    plot_combo->addData("Grip force [N]", data.statics.states.grip_force);
    plot_combo->addData("Pot. energy limbs [J]", data.statics.states.e_pot_limbs);
    plot_combo->addData("Pot. energy string [J]", data.statics.states.e_pot_string);
    plot_combo->setCombination(0, 1);

    auto tabs = new QTabWidget();
    tabs->addTab(numbers, "Characteristics");
    tabs->addTab(plot_shapes, "Shape");
    tabs->addTab(plot_stress, "Stress");
    tabs->addTab(plot_curvature, "Curvature");
    tabs->addTab(plot_energy, "Energy");
    tabs->addTab(plot_combo, "Other Plots");

    auto slider = new Slider(data.statics.states.draw_length, "Draw length [m]");
    slider->addJumpAction("Max. draw force", data.statics.max_draw_force_index);
    slider->addJumpAction("Max. grip force", data.statics.max_grip_force_index);
    slider->addJumpAction("Max. string force", data.statics.max_string_force_index);
    for (int i = 0; i < data.statics.max_stress_index.size(); ++i) {
        slider->addJumpAction(QString::fromStdString("Max. stress for layer:" + data.setup.limb_properties.layers[i].name), data.statics.max_stress_index[i].first);
    }

    QObject::connect(slider, &Slider::valueChanged, plot_shapes, &ShapePlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_stress, &StressPlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_curvature, &CurvaturePlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_energy, &EnergyPlot::setStateIndex);
    emit slider->valueChanged(0);

    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    vbox->setMargin(0);
    vbox->addWidget(tabs);
    vbox->addWidget(slider);
}

DynamicOutputWidget::DynamicOutputWidget(const OutputData& data)
{
    auto numbers = new NumberGrid();
    numbers->addColumn();
    numbers->addGroup("Performance");
    numbers->addValue("Final arrow velocity [m/s]", data.dynamics.final_vel_arrow);
    numbers->addValue("Efficiency", data.dynamics.efficiency);
    numbers->addGroup("Energy at arrow departure");
    numbers->addValue("Kinetic energy arrow [J]", data.dynamics.final_e_kin_arrow);
    numbers->addValue("Kinetic energy limbs [J]", data.dynamics.final_e_kin_limbs);
    numbers->addValue("Kinetic energy string [J]", data.dynamics.final_e_kin_string);
    numbers->addColumn();
    numbers->addGroup("Maximum absolute stresses");
    for (int i = 0; i < data.dynamics.max_stress_value.size(); ++i) {
        numbers->addValue(QString::fromStdString(data.setup.limb_properties.layers[i].name + " [Pa]"), data.dynamics.max_stress_value[i]);
    }
    numbers->addGroup("Maximum absolute forces");
    numbers->addValue("String force (total) [N]", data.dynamics.states.string_force[data.dynamics.max_string_force_index]);
    numbers->addValue("String force (strand) [N]", data.dynamics.states.strand_force[data.dynamics.max_string_force_index]);
    numbers->addValue("Grip force [N]", data.dynamics.states.grip_force[data.dynamics.max_grip_force_index]);

    auto plot_shapes = new ShapePlot(data.setup.limb_properties, data.dynamics.states, false);
    auto plot_stress = new StressPlot(data.setup.limb_properties, data.dynamics.states);
    auto plot_curvature = new CurvaturePlot(data.setup.limb_properties, data.dynamics.states);
    auto plot_energy = new EnergyPlot(data.dynamics.states, data.dynamics.states.time, "Time [s]");
    auto plot_combo = new ComboPlot();
    plot_combo->addData("Time [s]", data.dynamics.states.time);
    plot_combo->addData("Arrow position [m]", data.dynamics.states.pos_arrow);
    plot_combo->addData("Arrow velocity [m/s]", data.dynamics.states.vel_arrow);
    plot_combo->addData("Arrow acceleration [m/s²]", data.dynamics.states.acc_arrow);
    plot_combo->addData("String force (total) [N]", data.dynamics.states.string_force);
    plot_combo->addData("String force (strand) [N]", data.dynamics.states.strand_force);
    plot_combo->addData("Grip force [N]", data.dynamics.states.grip_force);
    plot_combo->addData("Pot. energy limbs [J]", data.dynamics.states.e_pot_limbs);
    plot_combo->addData("Kin. energy limbs [J]", data.dynamics.states.e_kin_limbs);
    plot_combo->addData("Pot. energy string [J]", data.dynamics.states.e_pot_string);
    plot_combo->addData("Kin. energy string [J]", data.dynamics.states.e_kin_string);
    plot_combo->addData("Kin. energy arrow [J]", data.dynamics.states.e_kin_arrow);
    plot_combo->setCombination(0, 1);

    auto tabs = new QTabWidget();
    tabs->addTab(numbers, "Characteristics");
    tabs->addTab(plot_shapes, "Shape");
    tabs->addTab(plot_stress, "Stress");
    tabs->addTab(plot_curvature, "Curvature");
    tabs->addTab(plot_energy, "Energy");
    tabs->addTab(plot_combo, "Other Plots");

    auto slider = new Slider(data.dynamics.states.time, "Time [s]");
    slider->addJumpAction("Arrow departure", data.dynamics.arrow_departure_index);
    slider->addJumpAction("Max. grip force", data.dynamics.max_grip_force_index);
    slider->addJumpAction("Max. string force", data.dynamics.max_string_force_index);
    for (int i = 0; i < data.dynamics.max_stress_index.size(); ++i) {
        slider->addJumpAction(QString::fromStdString("Max. stress for layer: " + data.setup.limb_properties.layers[i].name), data.dynamics.max_stress_index[i].first);
    }

    QObject::connect(slider, &Slider::valueChanged, plot_shapes, &ShapePlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_stress, &StressPlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_curvature, &CurvaturePlot::setStateIndex);
    QObject::connect(slider, &Slider::valueChanged, plot_energy, &EnergyPlot::setStateIndex);
    emit slider->valueChanged(0);

    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    vbox->setMargin(0);
    vbox->addWidget(tabs);
    vbox->addWidget(slider);
}
