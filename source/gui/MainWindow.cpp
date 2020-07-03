#include "MainWindow.hpp"
#include "SimulationDialog.hpp"
#include "RecentFilesMenu.hpp"
#include "HelpMenu.hpp"
#include "editors/BowEditor.hpp"
#include "config.hpp"
#include <nlohmann/json.hpp>

MainWindow::MainWindow()
    : editor(new BowEditor()),
      menu_recent(new RecentFilesMenu(this))
{
    // Actions
    action_new = new QAction(QIcon(":/icons/document-new.png"), "&New", this);
    QObject::connect(action_new, &QAction::triggered, this, &MainWindow::newFile);
    action_new->setShortcuts(QKeySequence::New);
    action_new->setMenuRole(QAction::NoRole);

    action_open = new QAction(QIcon(":/icons/document-open.png"), "&Open...", this);
    QObject::connect(action_open, &QAction::triggered, this, &MainWindow::open);
    action_open->setShortcuts(QKeySequence::Open);
    action_open->setMenuRole(QAction::NoRole);
    action_open->setEnabled(false);

    action_save = new QAction(QIcon(":/icons/document-save.png"), "&Save", this);
    QObject::connect(action_save, &QAction::triggered, this, &MainWindow::save);
    action_save->setShortcuts(QKeySequence::Save);
    action_save->setMenuRole(QAction::NoRole);
    action_save->setEnabled(false);

    action_save_as = new QAction(QIcon(":/icons/document-save-as.png"), "Save &As...", this);
    QObject::connect(action_save_as, &QAction::triggered, this, &MainWindow::saveAs);
    action_save_as->setShortcuts(QKeySequence::SaveAs);
    action_save_as->setMenuRole(QAction::NoRole);
    action_save_as->setEnabled(false);

    action_quit = new QAction(QIcon(":/icons/application-exit.png"), "&Quit", this);
    QObject::connect(action_quit, &QAction::triggered, this, &QWidget::close);
    action_quit->setShortcuts(QKeySequence::Quit);
    action_quit->setMenuRole(QAction::QuitRole);

    action_run_statics = new QAction(QIcon(":/icons/run-statics"), "&Statics...", this);
    QObject::connect(action_run_statics, &QAction::triggered, [&]{ runSimulation("--static"); });
    action_run_statics->setShortcut(Qt::Key_F5);
    action_run_statics->setMenuRole(QAction::NoRole);
    action_run_statics->setIconVisibleInMenu(true);
    action_run_statics->setEnabled(false);

    action_run_dynamics = new QAction(QIcon(":/icons/run-dynamics"), "&Dynamics...", this);
    QObject::connect(action_run_dynamics, &QAction::triggered, [&]{ runSimulation("--dynamic"); });
    action_run_dynamics->setShortcut(Qt::Key_F6);
    action_run_dynamics->setMenuRole(QAction::NoRole);
    action_run_dynamics->setIconVisibleInMenu(true);
    action_run_dynamics->setEnabled(false);

    // Recent file menu
    QObject::connect(menu_recent, &RecentFilesMenu::openRecent, this, &MainWindow::openRecent);

    // File menu
    auto menu_file = this->menuBar()->addMenu("&File");
    menu_file->addAction(action_new);
    menu_file->addAction(action_open);
    menu_file->addMenu(menu_recent);
    menu_file->addSeparator();
    menu_file->addAction(action_save);
    menu_file->addAction(action_save_as);
    menu_file->addSeparator();
    menu_file->addAction(action_quit);

    // Simulation menu
    auto menu_simulation = this->menuBar()->addMenu("&Simulate");
    menu_simulation->addAction(action_run_statics);
    menu_simulation->addAction(action_run_dynamics);

    // Toolbar
    this->setContextMenuPolicy(Qt::NoContextMenu);    // Disables context menu for hiding the toolbar
    auto toolbar = this->addToolBar("Tools");
    toolbar->setObjectName("MainToolBar");            // Necessary for saving the window state
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
    this->setWindowIcon(QIcon(":/icons/logo.ico"));
    this->setCentralWidget(editor);
    this->resize(INITIAL_SIZE);
    setCurrentFile(QString());

    // Load geometry and state
    QSettings settings;
    restoreState(settings.value("MainWindow/state").toByteArray());
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());

    // Set Window's modification indicator when data has changed
    QObject::connect(editor, &BowEditor::modified, [&]{
        InputData new_data = editor->getData();
        this->setModified(new_data != data);
    });

    // Set editor invisible until a file has been loaded
    editor->setVisible(false);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(optionalSave())
    {
        event->accept();

        // Save state and geometry
        QSettings settings;
        settings.setValue("MainWindow/state", saveState());
        settings.setValue("MainWindow/geometry", saveGeometry());
    }
    else
    {
        event->ignore();
    }
}

// Todo: Unify loadFile and saveFile?
bool MainWindow::loadFile(const QString& path)
{
    try
    {
        data = InputData(path.toStdString());
        editor->setData(data);
        setCurrentFile(path);
        setModified(false);
        return true;
    }
    catch(const std::exception& e)  // Todo
    {
        QMessageBox::critical(this, "Error", "Failed to open " + path + "\n" + e.what());
        return false;
    }
}

bool MainWindow::saveFile(const QString& path)
{
    try
    {
        data = editor->getData();
        data.save(path.toStdString());
        setCurrentFile(path);
        setModified(false);
        return true;
    }
    catch(const std::exception& e)  // Todo
    {
        QMessageBox::critical(this, "Error", "Failed to save " + path + "\n" + e.what());
        return false;
    }
}

void MainWindow::newFile()
{
    if(!saveAs())
        return;

    data = InputData();
    editor->setData(data);
    editor->setVisible(true);

    action_open->setEnabled(true);
    action_save->setEnabled(true);
    action_save_as->setEnabled(true);
    action_run_statics->setEnabled(true);
    action_run_dynamics->setEnabled(true);

    setModified(false);
}

void MainWindow::open()
{
    if(!optionalSave())
        return;

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter("Model Files (*.bow)");

    if(dialog.exec() == QDialog::Accepted)
        loadFile(dialog.selectedFiles().first());
}

void MainWindow::openRecent(const QString& path)
{
    if(!optionalSave())
        return;

    loadFile(path);
}

bool MainWindow::save()
{
    if(this->windowFilePath().isEmpty())
        return saveAs();

    return saveFile(this->windowFilePath());
}

bool MainWindow::saveAs()
{
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter("Bow Files (*.bow)");
    dialog.setDefaultSuffix("bow");
    dialog.selectFile(this->windowFilePath().isEmpty() ? DEFAULT_FILENAME : this->windowFilePath());

    if(dialog.exec() == QDialog::Accepted)
        return saveFile(dialog.selectedFiles().first());

    return false;
}

void MainWindow::runSimulation(const QString& flag)
{
    if(!save())
        return;

    // Generate output filename
    QFileInfo info(this->windowFilePath());
    QString output_file = info.absolutePath() + QDir::separator() + info.completeBaseName() + ".res";

    // Run Simulation, launch Post on results if successful
    SimulationDialog dialog(this, this->windowFilePath(), output_file, flag);
    if(dialog.exec() == QDialog::Accepted)
    {
        QProcess::startDetached(
            QDir(QCoreApplication::applicationDirPath()).filePath("virtualbow-post"),
            { output_file },
            QCoreApplication::applicationDirPath()
        );
    }
}

// Workaround to prevent QApplication::applicationDisplayName from being appended to every dialog
// https://bugreports.qt.io/browse/QTBUG-70382
void MainWindow::setModified(bool modified)
{
    QApplication::setApplicationDisplayName(Config::APPLICATION_DISPLAY_NAME_GUI);
    setWindowModified(modified);
    QTimer::singleShot(0, [](){QApplication::setApplicationDisplayName(QString::null);});
}

void MainWindow::setCurrentFile(const QString &path)
{
    QApplication::setApplicationDisplayName(Config::APPLICATION_DISPLAY_NAME_GUI);
    setWindowFilePath(path);
    if(!path.isEmpty())
    {
        menu_recent->addPath(path);
    }

    QTimer::singleShot(0, [](){QApplication::setApplicationDisplayName(QString::null);});
}

// true: Discard and save, false: Cancel and don't save
bool MainWindow::optionalSave()
{
    if(!this->isWindowModified())
        return true;

    auto pick = QMessageBox::warning(this, "Save Document?", "The document has been modified.\nDo you want to save your changes?",
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    switch(pick)
    {
        case QMessageBox::Save: return save();
        case QMessageBox::Discard: return true;
        case QMessageBox::Cancel: return false;
        default: return false;  // Can't happen
    }
}
