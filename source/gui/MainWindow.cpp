#include "MainWindow.hpp"
#include "SimulationDialog.hpp"
#include "RecentFilesMenu.hpp"
#include "HelpMenu.hpp"
#include "treeview/TreeView.hpp"
#include "propertyview/PropertyView.hpp"
#include "limbview/LimbView.hpp"
#include "viewmodel/MainViewModel.hpp"
#include "viewmodel/DataViewModel.hpp"
#include "gui/units/UnitDialog.hpp"
#include "config.hpp"

#include <QMenuBar>
#include <QToolBar>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>

#include <QDockWidget>
#include <QLabel>

MainWindow::MainWindow()
    : menu_open_recent(new RecentFilesMenu(this)),
      view_model(new MainViewModel())
{
    // Actions
    action_new = new QAction(QIcon(":/icons/document-new.svg"), "&New", this);
    QObject::connect(action_new, &QAction::triggered, this, &MainWindow::newFile);
    action_new->setShortcuts(QKeySequence::New);
    action_new->setMenuRole(QAction::NoRole);

    action_open = new QAction(QIcon(":/icons/document-open.svg"), "&Open...", this);
    QObject::connect(action_open, &QAction::triggered, this, &MainWindow::open);
    action_open->setShortcuts(QKeySequence::Open);
    action_open->setMenuRole(QAction::NoRole);

    action_save = new QAction(QIcon(":/icons/document-save.svg"), "&Save", this);
    QObject::connect(action_save, &QAction::triggered, this, &MainWindow::save);
    action_save->setShortcuts(QKeySequence::Save);
    action_save->setMenuRole(QAction::NoRole);

    action_save_as = new QAction(QIcon(":/icons/document-save-as.svg"), "Save &As...", this);
    QObject::connect(action_save_as, &QAction::triggered, this, &MainWindow::saveAs);
    action_save_as->setShortcuts(QKeySequence::SaveAs);
    action_save_as->setMenuRole(QAction::NoRole);

    action_quit = new QAction("&Quit", this);
    QObject::connect(action_quit, &QAction::triggered, this, &QWidget::close);
    action_quit->setShortcuts(QKeySequence::Quit);
    action_quit->setMenuRole(QAction::QuitRole);

    action_run_statics = new QAction(QIcon(":/icons/run-statics"), "&Statics...", this);
    QObject::connect(action_run_statics, &QAction::triggered, [&]{ runSimulation("--static"); });
    action_run_statics->setShortcut(Qt::Key_F5);
    action_run_statics->setMenuRole(QAction::NoRole);
    action_run_statics->setIconVisibleInMenu(true);

    action_run_dynamics = new QAction(QIcon(":/icons/run-dynamics"), "&Dynamics...", this);
    QObject::connect(action_run_dynamics, &QAction::triggered, [&]{ runSimulation("--dynamic"); });
    action_run_dynamics->setShortcut(Qt::Key_F6);
    action_run_dynamics->setMenuRole(QAction::NoRole);
    action_run_dynamics->setIconVisibleInMenu(true);

    action_set_units = new QAction("&Units...", this);
    QObject::connect(action_set_units, &QAction::triggered, this, [&]{
        UnitDialog dialog(this);
        dialog.exec();
    });
    action_set_units->setMenuRole(QAction::NoRole);

    // File menu
    auto menu_file = this->menuBar()->addMenu("&File");
    menu_file->addAction(action_new);
    menu_file->addAction(action_open);
    menu_file->addMenu(menu_open_recent);
    menu_file->addSeparator();
    menu_file->addAction(action_save);
    menu_file->addAction(action_save_as);
    menu_file->addSeparator();
    menu_file->addAction(action_quit);

    // Recent files menu
    QObject::connect(menu_open_recent, &RecentFilesMenu::openRecent, this, &MainWindow::openRecent);
    QObject::connect(menu_file, &QMenu::aboutToShow, [&]{ menu_open_recent->updateActions(); });

    // Simulation menu
    auto menu_simulation = this->menuBar()->addMenu("&Simulate");
    menu_simulation->addAction(action_run_statics);
    menu_simulation->addAction(action_run_dynamics);

    // Options menu
    auto menu_preferences = this->menuBar()->addMenu("&Options");
    menu_preferences->addAction(action_set_units);

    // Toolbar
    this->setContextMenuPolicy(Qt::NoContextMenu);    // Disables context menu for hiding the toolbar
    auto toolbar = this->addToolBar("Tools");
    toolbar->setObjectName("MainToolBar");            // Necessary for saving the window state
    toolbar->setAutoFillBackground(true);
    toolbar->setMovable(false);
    toolbar->addAction(action_new);
    toolbar->addAction(action_open);
    toolbar->addAction(action_save);
    toolbar->addAction(action_save_as);
    toolbar->addSeparator();
    toolbar->addAction(action_run_statics);
    toolbar->addAction(action_run_dynamics);

    // Help menu
    this->menuBar()->addMenu(new HelpMenu(this));

    dock_tree_view = new QDockWidget("Model");
    dock_tree_view->setObjectName("TreeView");
    dock_tree_view->setFeatures(QDockWidget::NoDockWidgetFeatures);

    dock_property_view = new QDockWidget("Properties");
    dock_property_view->setObjectName("PropertyView");
    dock_property_view->setFeatures(QDockWidget::NoDockWidgetFeatures);

    dock_plot_view = new QDockWidget("2D View");
    dock_plot_view->setObjectName("PlotView");
    dock_plot_view->setFeatures(QDockWidget::NoDockWidgetFeatures);

    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    addDockWidget(Qt::LeftDockWidgetArea, dock_tree_view);
    addDockWidget(Qt::LeftDockWidgetArea, dock_property_view);
    addDockWidget(Qt::BottomDockWidgetArea, dock_plot_view);

    dock_tree_view->setWidget(new TreeView(view_model->dataModel()));
    dock_property_view->setWidget(new PropertyView(view_model->dataModel()));
    dock_plot_view->setWidget(new QLabel("2D View"));
    this->setCentralWidget(new LimbView(view_model->dataModel()));

    // Connect window file path to view model
    this->setWindowFilePath(view_model->displayPath());
    QObject::connect(view_model, &MainViewModel::displayPathChanged, this, &MainWindow::setWindowFilePath);

    // Connect modification indicator to view model
    this->setWindowModified(view_model->isModified());
    QObject::connect(view_model, &MainViewModel::modificationChanged, this, &MainWindow::setWindowModified);

    // Main window
    this->setWindowIcon(QIcon(":/icons/logo.svg"));
    this->resize(DEFAULT_SIZE);    // Only on first run, on subsequent runs overwritten by user settings

    // Load geometry and state
    QSettings settings;
    restoreState(settings.value("MainWindow/state").toByteArray());
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());

    // Load unit settings
    UnitSystem::loadFromSettings(settings);    // TODO: Move to ViewModel
}

// Attempts to load the given file
// Returns true on success and false on failure
bool MainWindow::loadFromFile(const QString& path) {
    try {
        view_model->loadFile(path);         // Load data from path
        menu_open_recent->addPath(path);    // Add path to the menu of recently opened files
        return true;
    }
    catch(const std::exception& e) {
        QMessageBox::critical(this, "Error", "Failed to open " + path + "\n" + e.what());
        return false;
    }
}

// Attempts to save to the given file
// Returns true on success and false on failure
bool MainWindow::saveToFile(const QString& path) {
    try {
        view_model->saveFile(path);    // Save data to path
        menu_open_recent->addPath(path);    // Add path to the menu of recently used files
        return true;
    }
    catch(const std::exception& e) {
        QMessageBox::critical(this, "Error", "Failed to save " + path + "\n" + e.what());
        return false;
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if(optionalSaveModifications()) {
        // Changes successfully saved or discarded
        // Continue closing, save window state and geometry
        QSettings settings;
        settings.setValue("MainWindow/state", saveState());
        settings.setValue("MainWindow/geometry", saveGeometry());
        event->accept();
    }
    else {
        // Optional save dialog canceled, abort closing
        event->ignore();
    }
}

void MainWindow::newFile() {
    // Make sure any modifications have been saved or discarded
    if(!optionalSaveModifications()) {
        return;
    }
    view_model->loadDefaults();
}

void MainWindow::open() {
    // Make sure any modifications have been saved or discarded
    if(!optionalSaveModifications()) {
        return;
    }

    // Open file dialog, let user pick a new file name and location
    QString path = showOpenFileDialog();
    if(path.isEmpty()) {
        return;    // Abort if no result (canceled)
    }

    loadFromFile(path);    // Load the selected path
}

void MainWindow::openRecent(const QString& path) {
    // Make sure any modifications have been saved or discarded
    if(!optionalSaveModifications()) {
        return;
    }

    // Load editor content from the selected path
    loadFromFile(path);
}

bool MainWindow::save() {
    // Retrieve the current file path from the view model
    QString path = view_model->filePath();

    // If the path is empty, the data isn't associated with a file yet.
    // Let the user pick a location for the file in that case.
    if(path.isEmpty()) {
        path = showSaveFileDialog();
        if(path.isEmpty()) {
            return false;    // Abort if no result (canceled by the user)
        }
    }

    return saveToFile(path);
}

bool MainWindow::saveAs() {
    QString path = showSaveFileDialog();
    if(!path.isEmpty()) {
        return saveToFile(path);
    }

    return false;
}

void MainWindow::runSimulation(const QString& flag) {
    if(!save()) {
        return;
    }

    // Generate output filename
    QFileInfo info(this->windowFilePath());
    QString output_file = info.absolutePath() + QDir::separator() + info.completeBaseName() + ".res";

    // Run Simulation, launch Post on results if successful
    SimulationDialog dialog(this, this->windowFilePath(), output_file, flag);
    if(dialog.exec() == QDialog::Accepted) {
        QProcess::startDetached(
            QDir(QCoreApplication::applicationDirPath()).filePath("virtualbow-post"),
            { output_file },
            QCoreApplication::applicationDirPath()
        );
    }
}

// If there are modifications to the current document, ask the user whether to save, discard or cancel.
// Returns true when successfully saved or discarded, false when canceled.
bool MainWindow::optionalSaveModifications() {
    if(!this->isWindowModified()) {
        return true;    // No modifications to save
    }

    auto pick = QMessageBox::warning(this, "Save Changes?", "Do you want to save the changes to " + this->windowFilePath() + "?",
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    switch(pick) {
        case QMessageBox::Save: return save();
        case QMessageBox::Discard: return true;
        case QMessageBox::Cancel: return false;
        default: return false;  // Can't happen
    }
}

// Show a file dialog for selecting a model file to open.
// Returns the filepath if successful, an empty string otherwise.
QString MainWindow::showOpenFileDialog() {
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter("Model Files (*.bow)");

    if(dialog.exec() == QDialog::Accepted) {
        return dialog.selectedFiles().first();
    }

    return QString();
}

// Show a file dialog for selecting a model file to save to.
// Returns the filepath if successful, an empty string otherwise.
QString MainWindow::showSaveFileDialog() {
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter("Model Files (*.bow)");
    dialog.setDefaultSuffix("bow");
    dialog.selectFile(this->windowFilePath().isEmpty() ? DEFAULT_FILENAME : this->windowFilePath());

    if(dialog.exec() == QDialog::Accepted) {
        return dialog.selectedFiles().first();
    }

    return QString();
}
