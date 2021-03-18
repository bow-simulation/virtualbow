#include "MainWindow.hpp"
#include "SimulationDialog.hpp"
#include "RecentFilesMenu.hpp"
#include "HelpMenu.hpp"
#include "editors/ModelEditor.hpp"
#include "config.hpp"
#include <nlohmann/json.hpp>

MainWindow::MainWindow()
    : menu_open_recent(new RecentFilesMenu(this)),
      editor(new ModelEditor(units))
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
        UnitDialog dialog(this, units);
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

    // Preferences menu
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

    // Main window
    this->setWindowIcon(QIcon(":/icons/logo.svg"));
    this->setStyleSheet("QMainWindow { background-image:url(:/icons/background.png); background-position: center; background-repeat: no-repeat; }");
    this->menuBar()->setAutoFillBackground(true);
    this->setCentralWidget(editor);
    this->resize(DEFAULT_SIZE);
    setFilePath(QString());

    // Load geometry and state
    QSettings settings;
    restoreState(settings.value("MainWindow/state").toByteArray());
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());

    // Set Window's modification indicator when data has changed
    QObject::connect(editor, &ModelEditor::modified, [&]{
        InputData new_data = editor->getData();
        this->setModified(new_data != data);
    });
}

// Load input file from a file and show the contents in the editor
// Returns true on success and false on failure
bool MainWindow::loadFromFile(const QString& path) {
    try {
        // Load data from path, set editor and window state
        data = InputData(path.toStdString());
        editor->setData(data);
        setFilePath(path);
        setModified(false);

        // Add path to the menu of recently used files
        menu_open_recent->addPath(path);

        return true;
    }
    catch(const std::exception& e) {
        QMessageBox::critical(this, "Error", "Failed to open " + path + "\n" + e.what());
        return false;
    }
}

// Save the editor contents to a file
// Returns true on success and false on failure
bool MainWindow::saveToFile(const QString& path) {
    try {
        data = editor->getData();
        data.save(path.toStdString());
        setFilePath(path);
        setModified(false);

        // Add path to the menu of recently used files
        menu_open_recent->addPath(path);

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

    // Open file dialog, let user pick a new file name and location
    QString path = showSaveFileDialog();
    if(path.isEmpty()) {
        return;    // Abort if no result (canceled)
    }

    // Set editor to new default content and save to the selected path
    data = InputData();
    editor->setData(data);
    saveToFile(path);
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

    // Load editor content from the selected path
    loadFromFile(path);
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
    return saveToFile(this->windowFilePath());
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

// Set the path of the file currently opened in the editor.
// May be empty to signal that no file is opened.
void MainWindow::setFilePath(const QString &path) {
    // Set the file path of this window. Workaround to prevent QApplication::applicationDisplayName
    // from being appended to every dialog, see https://bugreports.qt.io/browse/QTBUG-70382
    QApplication::setApplicationDisplayName(Config::APPLICATION_DISPLAY_NAME_GUI);
    setWindowFilePath(path);
    QTimer::singleShot(0, [](){QApplication::setApplicationDisplayName(QString::null);});

    // If path is not empty, enable editing, saving and simulation
    bool enabled = !path.isEmpty();
    editor->setVisible(enabled);
    action_save->setEnabled(enabled);
    action_save_as->setEnabled(enabled);
    action_run_statics->setEnabled(enabled);
    action_run_dynamics->setEnabled(enabled);
}

// Set the modified property of this window. Workaround to prevent QApplication::applicationDisplayName
// from being appended to every dialog, see https://bugreports.qt.io/browse/QTBUG-70382
void MainWindow::setModified(bool modified) {
    QApplication::setApplicationDisplayName(Config::APPLICATION_DISPLAY_NAME_GUI);
    setWindowModified(modified);
    QTimer::singleShot(0, [](){QApplication::setApplicationDisplayName(QString::null);});
}
