#include "MainWindow.hpp"
#include "Application.hpp"
#include "ProgressDialog.hpp"
#include "input/editors/BowEditor.hpp"
#include "output/OutputDialog.hpp"
#include "bow/BowModel.hpp"
#include "config.hpp"
#include <thread>
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
    QObject::connect(action_run_statics, &QAction::triggered, [&]{ runSimulation(false); });    // Todo: Use std::bind

    auto action_run_dynamics = new QAction(QIcon(":/icons/run-dynamics"), "Dynamics...", this);
    action_run_dynamics->setShortcut(Qt::Key_F6);
    action_run_dynamics->setMenuRole(QAction::NoRole);
    QObject::connect(action_run_dynamics, &QAction::triggered, [&]{ runSimulation(true); });    // Todo: Use std::bind

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
    int maxFileNr = 10;
    for(auto i = 0; i < maxFileNr; ++i) {
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
    this->setWindowIcon(QIcon(":/icons/logo"));
    this->setCentralWidget(editor);
    setCurrentFile(QString());

    // Load geometry and state
    restoreState(Application::settings.value("MainWindow/state").toByteArray());
    restoreGeometry(Application::settings.value("MainWindow/geometry").toByteArray());

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

MainWindow::~MainWindow()
{
    // Save state and geometry
    Application::settings.setValue("MainWindow/state", saveState());
    Application::settings.setValue("MainWindow/geometry", saveGeometry());
    saveRecentFilePaths();
}

// Todo: Unify loadFile and saveFile?
bool MainWindow::loadFile(const QString& path)
{
    try
    {
        data.load(path.toStdString());
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(optionalSave())
    {
        event->accept();
    }
    else
    {
        event->ignore();
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
    if(current_file.isEmpty())
        return saveAs();

    return saveFile(current_file);
}

bool MainWindow::saveAs()
{
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter("Bow Files (*.bow)");
    dialog.setDefaultSuffix("bow");

    if(dialog.exec() == QDialog::Accepted)
        return saveFile(dialog.selectedFiles().first());

    return false;
}

void MainWindow::runSimulation(bool dynamic)
{
    ProgressDialog dialog(this);
    dialog.addProgressBar("Statics");
    if(dynamic)
    {
        dialog.addProgressBar("Dynamics");
    }

    const InputData& input = editor->getData();
    OutputData output;

    std::exception_ptr exception = nullptr;
    std::thread thread([&]
    {
        auto progress0 = [&](int p){
            QMetaObject::invokeMethod(&dialog, "setProgress", Qt::QueuedConnection, Q_ARG(int, 0), Q_ARG(int, p));
            return !dialog.isCanceled();
        };

        auto progress1 = [&](int p){
            QMetaObject::invokeMethod(&dialog, "setProgress", Qt::QueuedConnection, Q_ARG(int, 1), Q_ARG(int, p));
            return !dialog.isCanceled();
        };

        try
        {
            output = dynamic ? BowModel::run_dynamic_simulation(input, progress0, progress1)
                             : BowModel::run_static_simulation(input, progress0);

            QMetaObject::invokeMethod(&dialog, "accept", Qt::QueuedConnection);
        }
        catch(...)
        {
            exception = std::current_exception();
            QMetaObject::invokeMethod(&dialog, "reject", Qt::QueuedConnection);
        }
    });

    try
    {
        dialog.exec();
        thread.join();

        if(exception)
        {
            std::rethrow_exception(exception);
        }
    }
    catch(const std::exception& e)
    {
        QMessageBox::critical(this, "Error", e.what());
    }

    if(!dialog.isCanceled())
    {
        OutputDialog results(this, input, output);
        results.exec();
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, "About", QString()
        + "<strong><font size=\"6\">" + Config::APPLICATION_DISPLAY_NAME + "</font></strong><br>"
        + "Version " + Config::APPLICATION_VERSION + "<br><br>"
        + Config::APPLICATION_DESCRIPTION + "<br>"
        + "<a href=\"" + Config::APPLICATION_WEBSITE + "\">" + Config::APPLICATION_WEBSITE + "</a><br><br>"
        + "<small>" + Config::APPLICATION_COPYRIGHT + "<br>"
        + "Distributed under the " + Config::APPLICATION_LICENSE + "</small>"
    );
}

void MainWindow::setModified(bool modified)
{
    QApplication::setApplicationDisplayName(Config::APPLICATION_DISPLAY_NAME);
    setWindowModified(modified);
    QTimer::singleShot(0, [](){QApplication::setApplicationDisplayName(QString::null);});
}

void MainWindow::setCurrentFile(const QString &path)
{
    current_file = path;
    QApplication::setApplicationDisplayName(Config::APPLICATION_DISPLAY_NAME);
    setWindowFilePath(current_file.isEmpty() ? "untitled.bow" : current_file);
    if(!path.isEmpty())
    {
        updateRecentFilePaths(path);
    }

    QTimer::singleShot(0, [](){QApplication::setApplicationDisplayName(QString::null);});
}

bool MainWindow::optionalSave()    // true: Discard and save, false: Cancel and don't save
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
    int numfiles = Application::settings.beginReadArray("MainWindow/recentFiles");
    for(int i = 0; i < numfiles; i++) {
        Application::settings.setArrayIndex(i);
        recentFilePaths.append(Application::settings.value("path").toString());
    }
    Application::settings.endArray();
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
    Application::settings.beginWriteArray("MainWindow/recentFiles");
    for(int i = 0; i < recentFilePaths.size(); i++) {
        Application::settings.setArrayIndex(i);
        Application::settings.setValue("path", recentFilePaths.at(i));
    }
    Application::settings.endArray();
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
