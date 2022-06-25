#include "MainWindow.hpp"
#include "SimulationDialog.hpp"
#include "RecentFilesMenu.hpp"
#include "HelpMenu.hpp"
#include "treedock/TreeDock.hpp"
#include "editdock/EditDock.hpp"
#include "plotdock/PlotDock.hpp"
#include "limbview/LimbView.hpp"
#include "viewmodel/ViewModel.hpp"
#include "utils/UserSettings.hpp"
#include "UnitDialog.hpp"
#include "config.hpp"

#include <QMenuBar>
#include <QToolBar>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>

MainWindow::MainWindow()
    : view_model(new ViewModel()),
      menu_open_recent(new RecentFilesMenu(this))
{
    // Actions
    auto action_new = new QAction(QIcon(":/icons/document-new.svg"), "&New", this);
    QObject::connect(action_new, &QAction::triggered, this, &MainWindow::newFile);
    action_new->setShortcuts(QKeySequence::New);
    action_new->setMenuRole(QAction::NoRole);

    auto action_open = new QAction(QIcon(":/icons/document-open.svg"), "&Open...", this);
    QObject::connect(action_open, &QAction::triggered, this, &MainWindow::open);
    action_open->setShortcuts(QKeySequence::Open);
    action_open->setMenuRole(QAction::NoRole);

    auto action_save = new QAction(QIcon(":/icons/document-save.svg"), "&Save", this);
    QObject::connect(action_save, &QAction::triggered, this, &MainWindow::save);
    action_save->setShortcuts(QKeySequence::Save);
    action_save->setMenuRole(QAction::NoRole);

    auto action_save_as = new QAction(QIcon(":/icons/document-save-as.svg"), "Save &As...", this);
    QObject::connect(action_save_as, &QAction::triggered, this, &MainWindow::saveAs);
    action_save_as->setShortcuts(QKeySequence::SaveAs);
    action_save_as->setMenuRole(QAction::NoRole);

    auto action_quit = new QAction("&Quit", this);
    QObject::connect(action_quit, &QAction::triggered, this, &QWidget::close);
    action_quit->setShortcuts(QKeySequence::Quit);
    action_quit->setMenuRole(QAction::QuitRole);

    auto action_run_statics = new QAction(QIcon(":/icons/run-statics"), "&Statics...", this);
    QObject::connect(action_run_statics, &QAction::triggered, [&]{ runSimulation("--static"); });
    action_run_statics->setShortcut(Qt::Key_F5);
    action_run_statics->setMenuRole(QAction::NoRole);
    action_run_statics->setIconVisibleInMenu(true);

    auto action_run_dynamics = new QAction(QIcon(":/icons/run-dynamics"), "&Dynamics...", this);
    QObject::connect(action_run_dynamics, &QAction::triggered, [&]{ runSimulation("--dynamic"); });
    action_run_dynamics->setShortcut(Qt::Key_F6);
    action_run_dynamics->setMenuRole(QAction::NoRole);
    action_run_dynamics->setIconVisibleInMenu(true);

    auto action_set_units = new QAction("&Units...", this);
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

    // Dock widgets and main widget
    this->setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    this->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    auto limb_view = new LimbView(view_model);
    auto tree_dock = new TreeDock(view_model);
    auto edit_dock = new EditDock();
    auto plot_dock = new PlotDock();

    QObject::connect(tree_dock, &TreeDock::currentEditorChanged, edit_dock, &EditDock::showEditor);
    QObject::connect(tree_dock, &TreeDock::currentPlotChanged, plot_dock, &PlotDock::showPlot);

    this->setCentralWidget(limb_view);
    this->addDockWidget(Qt::LeftDockWidgetArea, tree_dock);
    this->addDockWidget(Qt::LeftDockWidgetArea, edit_dock);
    this->addDockWidget(Qt::BottomDockWidgetArea, plot_dock);

    // Connect window file path to view model
    this->setWindowFilePath(view_model->displayPath());
    QObject::connect(view_model, &ViewModel::displayPathChanged, this, &MainWindow::setWindowFilePath);

    // Connect modification indicator to view model
    this->setWindowModified(view_model->isModified());
    QObject::connect(view_model, &ViewModel::modificationChanged, this, &MainWindow::setWindowModified);

    // Main window
    this->setWindowIcon(QIcon(":/icons/logo.svg"));
    this->resize({1000, 600});    // Initial size, overwritten by stored window geometry if present

    // Load geometry and state
    UserSettings settings;
    restoreState(settings.value("MainWindow/state").toByteArray());
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());

    // Load unit settings
    Quantities::loadFromSettings(settings);    // TODO: Move to ViewModel

    // Load defaults
    newFile();
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
        QMessageBox::critical(this, "Error", "Failed to open " + path + ":\n" + e.what());
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
        QMessageBox::critical(this, "Error", "Failed to save " + path + ":\n" + e.what());
        return false;
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if(optionalSaveModifications()) {
        // Changes successfully saved or discarded
        // Continue closing, save window state and geometry
        UserSettings settings;
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
    dialog.selectFile(view_model->displayPath());

    if(dialog.exec() == QDialog::Accepted) {
        return dialog.selectedFiles().first();
    }

    return QString();
}
