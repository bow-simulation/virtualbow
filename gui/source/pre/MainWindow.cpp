#include "MainWindow.hpp"
#include "SimulationDialog.hpp"
#include "RecentFilesMenu.hpp"
#include "HelpMenu.hpp"
#include "post/ResultWindow.hpp"
#include "pre/models/units/UnitSystem.hpp"
#include "views/docks/TreeDock.hpp"
#include "views/docks/EditDock.hpp"
#include "views/docks/PlotDock.hpp"
#include "views/limb3d/LimbView.hpp"
#include "models/MainModel.hpp"
#include "utils/UserSettings.hpp"
#include "UnitDialog.hpp"
#include <QMenuBar>
#include <QToolBar>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>

QString MainWindow::DEFAULT_NAME = "Unnamed";

MainWindow::MainWindow()
    : mainModel(new MainModel()),
      menuOpenRecent(new RecentFilesMenu(this))
{
    // Actions for manin menus and toolbar

    auto actionNew = new QAction(QIcon(":/icons/document-new.svg"), "&New", this);
    QObject::connect(actionNew, &QAction::triggered, this, &MainWindow::newFile);
    actionNew->setShortcuts(QKeySequence::New);
    actionNew->setMenuRole(QAction::NoRole);

    auto actionOpen = new QAction(QIcon(":/icons/document-open.svg"), "&Open...", this);
    QObject::connect(actionOpen, &QAction::triggered, this, &MainWindow::open);
    actionOpen->setShortcuts(QKeySequence::Open);
    actionOpen->setMenuRole(QAction::NoRole);

    auto actionSave = new QAction(QIcon(":/icons/document-save.svg"), "&Save", this);
    QObject::connect(actionSave, &QAction::triggered, this, &MainWindow::save);
    actionSave->setShortcuts(QKeySequence::Save);
    actionSave->setMenuRole(QAction::NoRole);

    auto actionSaveAs = new QAction(QIcon(":/icons/document-save-as.svg"), "Save &As...", this);
    QObject::connect(actionSaveAs, &QAction::triggered, this, &MainWindow::saveAs);
    actionSaveAs->setShortcuts(QKeySequence::SaveAs);
    actionSaveAs->setMenuRole(QAction::NoRole);

    auto actionQuit = new QAction("&Quit", this);
    QObject::connect(actionQuit, &QAction::triggered, this, &QWidget::close);
    actionQuit->setShortcuts(QKeySequence::Quit);
    actionQuit->setMenuRole(QAction::QuitRole);

    auto actionRunStatics = new QAction(QIcon(":/icons/run-statics"), "&Statics...", this);
    QObject::connect(actionRunStatics, &QAction::triggered, [&]{ runSimulation(Mode::Static); });
    actionRunStatics->setShortcut(Qt::Key_F5);
    actionRunStatics->setMenuRole(QAction::NoRole);
    actionRunStatics->setIconVisibleInMenu(true);

    auto actionRunDynamics = new QAction(QIcon(":/icons/run-dynamics"), "&Dynamics...", this);
    QObject::connect(actionRunDynamics, &QAction::triggered, [&]{ runSimulation(Mode::Dynamic); });
    actionRunDynamics->setShortcut(Qt::Key_F6);
    actionRunDynamics->setMenuRole(QAction::NoRole);
    actionRunDynamics->setIconVisibleInMenu(true);

    auto actionEditUnits = new QAction("&Units...", this);
    QObject::connect(actionEditUnits, &QAction::triggered, this, [&]{
        auto *dialog = new UnitDialog(this);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->show();
    });
    actionEditUnits->setMenuRole(QAction::NoRole);

    // Some actions are only available if a bow model is present

    actionSave->setEnabled(mainModel->hasBow());
    QObject::connect(mainModel, &MainModel::hasBowModelChanged, actionSave, &QAction::setEnabled);

    actionSaveAs->setEnabled(mainModel->hasBow());
    QObject::connect(mainModel, &MainModel::hasBowModelChanged, actionSaveAs, &QAction::setEnabled);

    actionRunStatics->setEnabled(mainModel->hasBow());
    QObject::connect(mainModel, &MainModel::hasBowModelChanged, actionRunStatics, &QAction::setEnabled);

    actionRunDynamics->setEnabled(mainModel->hasBow());
    QObject::connect(mainModel, &MainModel::hasBowModelChanged, actionRunDynamics, &QAction::setEnabled);

    // File menu
    auto menuFile = this->menuBar()->addMenu("&File");
    menuFile->addAction(actionNew);
    menuFile->addAction(actionOpen);
    menuFile->addMenu(menuOpenRecent);
    menuFile->addSeparator();
    menuFile->addAction(actionSave);
    menuFile->addAction(actionSaveAs);
    menuFile->addSeparator();
    menuFile->addAction(actionQuit);

    // Recent files menu
    QObject::connect(menuOpenRecent, &RecentFilesMenu::openRecent, this, &MainWindow::openRecent);
    QObject::connect(menuFile, &QMenu::aboutToShow, [&]{ menuOpenRecent->updateActions(); });

    // Simulation menu
    auto menuSimulation = this->menuBar()->addMenu("&Simulate");
    menuSimulation->addAction(actionRunStatics);
    menuSimulation->addAction(actionRunDynamics);

    // Options menu
    auto menuPreferences = this->menuBar()->addMenu("&Options");
    menuPreferences->addAction(actionEditUnits);

    // Toolbar
    this->setContextMenuPolicy(Qt::NoContextMenu);    // Disables context menu for hiding the toolbar
    auto toolbar = this->addToolBar("Tools");
    toolbar->setObjectName("MainToolBar");            // Necessary for saving the window state
    toolbar->setAutoFillBackground(true);
    toolbar->setMovable(false);
    toolbar->addAction(actionNew);
    toolbar->addAction(actionOpen);
    toolbar->addAction(actionSave);
    toolbar->addAction(actionSaveAs);
    toolbar->addSeparator();
    toolbar->addAction(actionRunStatics);
    toolbar->addAction(actionRunDynamics);

    // Help menu
    this->menuBar()->addMenu(new HelpMenu(this));

    // Dock widgets and main widget
    this->setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    this->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    auto limbView = new LimbView(mainModel);
    auto treeDock = new TreeDock(mainModel);
    auto editDock = new EditDock(mainModel);
    auto plotDock = new PlotDock(mainModel);

    this->setCentralWidget(limbView);
    this->addDockWidget(Qt::LeftDockWidgetArea, treeDock);
    this->addDockWidget(Qt::LeftDockWidgetArea, editDock);
    this->addDockWidget(Qt::BottomDockWidgetArea, plotDock);

    // Connect window file path to view model
    this->setWindowFilePath(displayPath());
    QObject::connect(mainModel, &MainModel::currentFileChanged, this, [&](){
        setWindowFilePath(displayPath());
    });

    // Connect modification indicator to view model
    this->setWindowModified(mainModel->hasUnsavedWork());
    QObject::connect(mainModel, &MainModel::hasUnsavedWorkChanged, this, &MainWindow::setWindowModified);

    // Main window
    this->setWindowIcon(QIcon(":/icons/logo.svg"));
    this->resize({1200, 800});    // Initial size, overwritten by stored window geometry if present

    // Load geometry and state
    UserSettings settings;
    restoreState(settings.value("MainWindow/state").toByteArray());    // TODO: Reenable
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());    // TODO: Reenable

    // Load unit settings
    Quantities::loadFromSettings(settings);    // TODO: Move to mainModel?
}

// Attempts to load the given file
// Returns true on success and false on failure
bool MainWindow::loadFromFile(const QString& path) {
    try {
        mainModel->loadFile(path);        // Load data from path
        menuOpenRecent->addPath(path);    // Add path to the menu of recently opened files
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
        mainModel->saveFile(path);        // Save data to path
        menuOpenRecent->addPath(path);    // Add path to the menu of recently used files
        return true;
    }
    catch(const std::exception& e) {
        QMessageBox::critical(this, "Error", "Failed to save " + path + ":\n" + e.what());
        return false;
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
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
    mainModel->newFile();
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
    QString path = mainModel->currentFile();

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

void MainWindow::runSimulation(Mode mode) {
    // Return if there is nothing to simulate
    if(!mainModel->hasBow()) {
        return;
    }

    // Run Simulation, launch Post on results if successful
    SimulationDialog dialog(this, mainModel->getBow(), mode);
    if(dialog.exec() == QDialog::Accepted) {
        auto window = new ResultWindow(this->windowFilePath(), dialog.getResult(), this);
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->show();
    }
}

// If there are modifications to the current document, ask the user whether to save, discard or cancel.
// Returns true when successfully saved or discarded, false when canceled.
bool MainWindow::optionalSaveModifications() {
    // Make sure all editing is finished
    submitChanges();

    // Return true if no unsaved changes were made
    if(!this->isWindowModified()) {
        return true;
    }

    // Otherwise ask user whether to save changes
    auto pick = QMessageBox::warning(this, "Save Changes?", "Do you want to save the changes to " + this->windowFilePath() + "?",
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    switch(pick) {
        case QMessageBox::Save: return save();
        case QMessageBox::Discard: return true;
        case QMessageBox::Cancel: return false;
        default: return false;  // Can't happen
    }
}

// If a widget has keyboard focus, clear it to submit any pending changes
void MainWindow::submitChanges() {
    QWidget* widget = QApplication::focusWidget();
    if(widget != nullptr) {
        widget->clearFocus();
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
    dialog.selectFile(displayPath());

    if(dialog.exec() == QDialog::Accepted) {
        return dialog.selectedFiles().first();
    }

    return QString();
}

// Filename to display at the top of the window, which is either the actual name of the current file
// or the default name if no file is loaded
QString MainWindow::displayPath() {
    if(!mainModel->currentFile().isEmpty()) {
        return mainModel->currentFile();
    }

    if(mainModel->hasBow()) {
        return DEFAULT_NAME;
    }

    return "";
}
