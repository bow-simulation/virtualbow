#include "MainWindow.hpp"
#include "Settings.hpp"
#include "SimulationDialog.hpp"
#include "editors/BowEditor.hpp"
#include "config.hpp"
#include <nlohmann/json.hpp>

MainWindow::MainWindow()
    : editor(new BowEditor())
{
    // Actions
    auto action_new = new QAction(QIcon::fromTheme("document-new", QIcon(":/icons/document-new.png")), "&New", this);
    QObject::connect(action_new, &QAction::triggered, this, &MainWindow::newFile);
    action_new->setShortcuts(QKeySequence::New);
    action_new->setMenuRole(QAction::NoRole);

    auto action_open = new QAction(QIcon::fromTheme("document-open", QIcon(":/icons/document-open.png")), "&Open...", this);
    QObject::connect(action_open, &QAction::triggered, this, &MainWindow::open);
    action_open->setShortcuts(QKeySequence::Open);
    action_open->setMenuRole(QAction::NoRole);

    auto action_save = new QAction(QIcon::fromTheme("document-save", QIcon(":/icons/document-save.png")), "&Save", this);
    QObject::connect(action_save, &QAction::triggered, this, &MainWindow::save);
    action_save->setShortcuts(QKeySequence::Save);
    action_save->setMenuRole(QAction::NoRole);

    auto action_save_as = new QAction(QIcon::fromTheme("document-save-as", QIcon(":/icons/document-save-as.png")), "Save &As...", this);
    QObject::connect(action_save_as, &QAction::triggered, this, &MainWindow::saveAs);
    action_save_as->setShortcuts(QKeySequence::SaveAs);
    action_save_as->setMenuRole(QAction::NoRole);

    auto action_exit = new QAction(QIcon::fromTheme("application-exit", QIcon(":/icons/application-exit.png")), "&Quit", this);
    QObject::connect(action_exit, &QAction::triggered, this, &QWidget::close);
    action_exit->setShortcuts(QKeySequence::Quit);
    action_exit->setMenuRole(QAction::QuitRole);

    auto action_run_statics = new QAction(QIcon(":/icons/run-statics"), "Statics...", this);
    action_run_statics->setShortcut(Qt::Key_F5);
    action_run_statics->setMenuRole(QAction::NoRole);
    QObject::connect(action_run_statics, &QAction::triggered, [&]{ runSimulation("--static"); });    // Todo: Use std::bind

    auto action_run_dynamics = new QAction(QIcon(":/icons/run-dynamics"), "Dynamics...", this);
    action_run_dynamics->setShortcut(Qt::Key_F6);
    action_run_dynamics->setMenuRole(QAction::NoRole);
    QObject::connect(action_run_dynamics, &QAction::triggered, [&]{ runSimulation("--dynamic"); });    // Todo: Use std::bind

    auto action_about = new QAction(QIcon::fromTheme("dialog-information", QIcon(":/icons/dialog-information.png")), "&About...", this);
    connect(action_about, &QAction::triggered, this, &MainWindow::about);
    action_about->setMenuRole(QAction::AboutRole);

    // File menu
    auto menu_file = this->menuBar()->addMenu("&File");
    menu_file->addAction(action_new);
    menu_file->addAction(action_open);
    auto menu_recentfiles = menu_file->addMenu("&Recent Files");
    menu_recentfiles->setObjectName("menu_recentfiles");
    menu_recentfiles->setToolTipsVisible(true);
    for(int i = 0; i < N_RECENT_FILES; ++i) {
        QAction* action_recentfile = new QAction(this);
        action_recentfile->setVisible(false);
        QObject::connect(action_recentfile, &QAction::triggered, this, &MainWindow::openRecent);
        recentFileActions.append(action_recentfile);
        menu_recentfiles->addAction(action_recentfile);
    }
    menu_file->addSeparator();
    menu_file->addAction(action_save);
    menu_file->addAction(action_save_as);
    menu_file->addSeparator();
    menu_file->addAction(action_exit);

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
    auto menu_help = this->menuBar()->addMenu("&Help");
    menu_help->addAction(action_about);

    // Main window
    QIcon icon;
    icon.addFile(":/icons/logo-16.png");
    icon.addFile(":/icons/logo-24.png");
    icon.addFile(":/icons/logo-32.png");
    icon.addFile(":/icons/logo-48.png");
    icon.addFile(":/icons/logo-64.png");

    this->setWindowIcon(QIcon(":/icons/logo.ico"));
    this->setCentralWidget(editor);
    setCurrentFile(QString());

    // Load geometry and state
    restoreState(SETTINGS.value("MainWindow/state").toByteArray());
    restoreGeometry(SETTINGS.value("MainWindow/geometry").toByteArray());

    // Set Window's modification indicator when data has changed
    QObject::connect(editor, &BowEditor::modified, [&]{
        InputData new_data = editor->getData();
        this->setModified(new_data != data);
    });

    // Populate the recent files menu
    readRecentFilePaths();
    updateRecentActionList();

    // Set initial input data
    newFile();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(optionalSave())
    {
        event->accept();

        // Save state and geometry
        SETTINGS.setValue("MainWindow/state", saveState());
        SETTINGS.setValue("MainWindow/geometry", saveGeometry());
        saveRecentFilePaths();
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
        QMessageBox::critical(this, "", "Failed to open " + path + "\n" + e.what());  // Todo: Detailed error message
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
        QMessageBox::critical(this, "", "Failed to save " + path + "\n" + e.what());  // Todo: Detailed error message
        return false;
    }
}

void MainWindow::newFile()
{
    if(!optionalSave())
        return;

    // Create new file and set version correctly
    data = InputData();
    data.meta.version = Config::APPLICATION_VERSION;
    editor->setData(data);

    setCurrentFile(QString());
    setModified(false);
}

void MainWindow::open()
{
    if(!optionalSave())
        return;

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter("Bow Files (*.bow)");

    if(dialog.exec() == QDialog::Accepted)
        loadFile(dialog.selectedFiles().first());
}

void MainWindow::openRecent(){
    if(!optionalSave())
        return;

    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        loadFile(action->data().toString());
}

bool MainWindow::save()
{
    if(currentFile.isEmpty())
        return saveAs();

    return saveFile(currentFile);
}

bool MainWindow::saveAs()
{
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter("Bow Files (*.bow)");
    dialog.setDefaultSuffix("bow");
    dialog.selectFile(currentFile.isEmpty() ? DEFAULT_FILENAME : currentFile);

    if(dialog.exec() == QDialog::Accepted)
        return saveFile(dialog.selectedFiles().first());

    return false;
}

void MainWindow::runSimulation(const QString& flag)
{
    // Make sure the current data is saved to a file
    if(currentFile.isEmpty())
    {
        int pick = QMessageBox::warning(this, "", "The document needs to be saved before running a simulation.\nDo you want to save now?",
                                        QMessageBox::Yes | QMessageBox::Cancel);
        if(pick != QMessageBox::Yes)
            return;

        if(!saveAs())
            return;
    }
    else
    {
        save();
    }

    // Generate output filename
    QFileInfo info(currentFile);
    QString output_file = info.absolutePath() + QDir::separator() + info.completeBaseName() + ".vbr";

    // Run Simulation, launch Post on results if successful
    SimulationDialog dialog(this, currentFile, output_file, flag);
    if(dialog.exec() == QDialog::Accepted)
    {
        QProcess *process = new QProcess(this);
        process->setWorkingDirectory(QCoreApplication::applicationDirPath());
        process->setProgram("virtualbow-post");
        process->setArguments({ output_file });
        process->startDetached();
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, "About", QString()
        + "<strong><font size=\"6\">" + Config::APPLICATION_NAME_GUI + "</font></strong><br>"
        + "Version " + Config::APPLICATION_VERSION + "<br><br>"
        + Config::APPLICATION_DESCRIPTION + "<br>"
        + "<a href=\"" + Config::APPLICATION_WEBSITE + "\">" + Config::APPLICATION_WEBSITE + "</a><br><br>"
        + "<small>" + Config::APPLICATION_COPYRIGHT + "<br>"
        + "Distributed under the " + Config::APPLICATION_LICENSE + "</small>"
    );
}

// Workaround to prevent QApplication::applicationDisplayName from being appended to every dialog
// https://bugreports.qt.io/browse/QTBUG-70382
void MainWindow::setModified(bool modified)
{
    QApplication::setApplicationDisplayName(Config::APPLICATION_NAME_GUI);
    setWindowModified(modified);
    QTimer::singleShot(0, [](){QApplication::setApplicationDisplayName(QString::null);});
}

void MainWindow::setCurrentFile(const QString &path)
{
    currentFile = path;
    QApplication::setApplicationDisplayName(Config::APPLICATION_NAME_GUI);
    setWindowFilePath(currentFile.isEmpty() ? DEFAULT_FILENAME : currentFile);
    if(!path.isEmpty())
    {
        updateRecentFilePaths(path);
    }

    QTimer::singleShot(0, [](){QApplication::setApplicationDisplayName(QString::null);});
}

// true: Discard and save, false: Cancel and don't save
bool MainWindow::optionalSave()
{
    if(!this->isWindowModified())
        return true;

    auto pick = QMessageBox::warning(this, "", "The document has been modified.\nDo you want to save your changes?",
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    switch(pick)
    {
        case QMessageBox::Save: return save();
        case QMessageBox::Discard: return true;
        case QMessageBox::Cancel: return false;
    }
}

void MainWindow::readRecentFilePaths()
{
    recentFilePaths.clear();
    int numfiles = SETTINGS.beginReadArray("MainWindow/recentFiles");
    for(int i = 0; i < numfiles; i++) {
        SETTINGS.setArrayIndex(i);
        recentFilePaths.append(SETTINGS.value("path").toString());
    }
    SETTINGS.endArray();
}

void MainWindow::updateRecentFilePaths(const QString& path)
{
    recentFilePaths.removeAll(path);
    recentFilePaths.prepend(path);
    while(recentFilePaths.size() > recentFileActions.size())
        recentFilePaths.removeLast();
}

void MainWindow::saveRecentFilePaths()
{
    SETTINGS.beginWriteArray("MainWindow/recentFiles");
    for(int i = 0; i < recentFilePaths.size(); i++) {
        SETTINGS.setArrayIndex(i);
        SETTINGS.setValue("path", recentFilePaths.at(i));
    }
    SETTINGS.endArray();
}

void MainWindow::updateRecentActionList()
{
    auto actionIter = recentFileActions.begin();
    auto fileIter = recentFilePaths.begin();

    for(; actionIter != recentFileActions.end() && fileIter != recentFilePaths.end(); actionIter++, fileIter++) {
        QString strippedName = QFileInfo(*fileIter).fileName();
        (*actionIter)->setText(strippedName);
        (*actionIter)->setToolTip(*fileIter);
        (*actionIter)->setData(*fileIter);
        (*actionIter)->setVisible(true);
    }

    for(; actionIter != recentFileActions.end(); actionIter++)
        (*actionIter)->setVisible(false);

    auto menu_recentfiles = MainWindow::findChild<QMenu*>("menu_recentfiles");
    if(menu_recentfiles) {
        menu_recentfiles->setEnabled(recentFilePaths.size() > 0);
    }
}
