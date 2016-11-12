#include "MainWindow.hpp"
#include "BowEditor.hpp"
#include "SettingsDialog.hpp"
#include "CommentsDialog.hpp"
#include "ProgressDialog.hpp"
#include "OutputDialog.hpp"
#include "../model/BowModel.hpp"
#include <thread>

MainWindow:: MainWindow()
    : input(":/bows/default.bow"),
      editor(new BowEditor(input))
{
    // Actions
    QAction* action_new = new QAction(QIcon(":/icons/document-new"), "&New", this);
    QObject::connect(action_new, &QAction::triggered, this, &MainWindow::newFile);
    action_new->setShortcuts(QKeySequence::New);
    action_new->setMenuRole(QAction::NoRole);

    QAction* action_open = new QAction(QIcon(":/icons/document-open"), "&Open...", this);
    QObject::connect(action_open, &QAction::triggered, this, &MainWindow::open);
    action_open->setShortcuts(QKeySequence::Open);
    action_open->setMenuRole(QAction::NoRole);

    QAction* action_save = new QAction(QIcon(":/icons/document-save"), "&Save", this);
    QObject::connect(action_save, &QAction::triggered, this, &MainWindow::save);
    action_save->setShortcuts(QKeySequence::Save);
    action_save->setMenuRole(QAction::NoRole);

    QAction* action_save_as = new QAction(QIcon(":/icons/document-save-as"), "Save &As...", this);
    QObject::connect(action_save_as, &QAction::triggered, this, &MainWindow::saveAs);
    action_save_as->setShortcuts(QKeySequence::SaveAs);
    action_save_as->setMenuRole(QAction::NoRole);

    QAction* action_exit = new QAction(QIcon(":/icons/application-exit"), "&Quit", this);
    QObject::connect(action_exit, &QAction::triggered, this, &QWidget::close);
    action_exit->setShortcuts(QKeySequence::Quit);
    action_exit->setMenuRole(QAction::QuitRole);

    QAction* action_settings = new QAction(QIcon(":/icons/applications-system"), "Settings...", this);
    QObject::connect(action_settings, &QAction::triggered, this, &MainWindow::settings);
    action_settings->setMenuRole(QAction::NoRole);

    QAction* action_notes = new QAction(QIcon(":/icons/comments"), "Comments...", this);
    QObject::connect(action_notes, &QAction::triggered, this, &MainWindow::comments);
    action_notes->setMenuRole(QAction::NoRole);

    QAction* action_run_statics = new QAction(QIcon(":/icons/arrow-yellow"), "Statics...", this);
    action_run_statics->setShortcut(Qt::Key_F5);
    action_run_statics->setMenuRole(QAction::NoRole);
    QObject::connect(action_run_statics, &QAction::triggered, [&](){ runSimulation(false); });    // Todo: Use std::bind

    QAction* action_run_dynamics = new QAction(QIcon(":/icons/arrow-green"), "Dynamics...", this);
    action_run_dynamics->setShortcut(Qt::Key_F6);
    action_run_dynamics->setMenuRole(QAction::NoRole);
    QObject::connect(action_run_dynamics, &QAction::triggered, [&](){ runSimulation(true); });    // Todo: Use std::bind

    QAction* action_about = new QAction(QIcon(":/icons/dialog-information"), "&About...", this);
    connect(action_about, &QAction::triggered, this, &MainWindow::about);
    action_about->setMenuRole(QAction::AboutRole);

    // File menu
    QMenu* menu_file = this->menuBar()->addMenu("&File");
    menu_file->addAction(action_new);
    menu_file->addAction(action_open);
    menu_file->addSeparator();
    menu_file->addAction(action_save);
    menu_file->addAction(action_save_as);
    menu_file->addSeparator();
    menu_file->addAction(action_exit);

    // Edit menu
    QMenu* menu_edit = this->menuBar()->addMenu("&Edit");
    menu_edit->addAction(action_settings);
    menu_edit->addAction(action_notes);

    // Simulation menu
    QMenu* menu_simulation = this->menuBar()->addMenu("&Simulate");
    menu_simulation->addAction(action_run_statics);
    menu_simulation->addAction(action_run_dynamics);

    // File toolbar
    QToolBar* toolbar_file = this->addToolBar("File");
    toolbar_file->addAction(action_new);
    toolbar_file->addAction(action_open);
    toolbar_file->addAction(action_save);
    toolbar_file->addAction(action_save_as);

    // Edit toolbar
    QToolBar* toolbar_edit = this->addToolBar("Edit");
    toolbar_edit->addAction(action_settings);
    toolbar_edit->addAction(action_notes);

    // Simulation toolbar
    QToolBar* toolbar_simulation = this->addToolBar("Simulate");
    toolbar_simulation->addAction(action_run_statics);
    toolbar_simulation->addAction(action_run_dynamics);

    // Help menu
    QMenu* menu_help = this->menuBar()->addMenu("&Help");
    menu_help->addAction(action_about);

    // Main window
    QObject::connect(&input, &Document::stateChanged, this, &QMainWindow::setWindowModified);
    this->setWindowIcon(QIcon(":/icons/logo"));
    this->setCentralWidget(editor);
    setCurrentFile(QString());
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

    input.load(":/bows/default.bow");
    setCurrentFile(QString());
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

// Todo: settings, notes and about dialogs as lambdas in constructor?
void MainWindow::settings()
{
    SettingsDialog dialog(this, input);
    dialog.exec();
}

void MainWindow::comments()
{
    CommentsDialog dialog(this, input);
    dialog.exec();
}

void MainWindow::runSimulation(bool dynamics)
{
    OutputData output;
    BowModel model(input, output);

    ProgressDialog dialog(this);
    dialog.addTask("Statics", [&](TaskState& task)
    {
        model.simulate_setup();
        model.simulate_statics(task);
    });

    if(dynamics)
    {
        dialog.addTask("Dynamics", [&](TaskState& task)
        {
            model.simulate_dynamics(task);
        });
    }

    try
    {
        if(dialog.exec() == QDialog::Accepted)
        {
            OutputDialog results(this, output);
            results.exec();
        }
    }
    catch(const std::runtime_error& e)
    {
        QMessageBox::critical(this, "Error", e.what());
    }
}

void MainWindow::about()
{
    QMessageBox msgbox(this);
    msgbox.setIconPixmap(QPixmap(":/icons/logo"));
    msgbox.setWindowTitle("About");
    msgbox.setText("<h2>" + QGuiApplication::applicationDisplayName() + "</h2>"
                   "<h4>Version " + QGuiApplication::applicationVersion() + "</h4>"
                   "Bow and arrow physics simulation<br>"
                   "<a href=\"" + QGuiApplication::organizationDomain() + "\">" + QGuiApplication::organizationDomain() + "</a><br><br>"
                   "Copyright (C) 2016 Stefan Pfeifer<br>"
                   "Distributed under the GNU General Public License v3.0");

    msgbox.exec();
}

void MainWindow::setCurrentFile(const QString &file_name)
{
    current_file = file_name;
    setWindowFilePath(current_file.isEmpty() ? "untitled.bow" : current_file);
}

bool MainWindow::optionalSave()    // true: Discard, false: Cancel
{
    if(!input.isModified())
        return true;

    auto pick = QMessageBox::warning(this, "", "The document has been modified.\nDo you want to save your changes?",
                                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    switch(pick)
    {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Discard:
        return true;
    case QMessageBox::Cancel:
        return false;
    }
}

// Todo: Unify loadFile and saveFile?
bool MainWindow::loadFile(const QString &file_name)
{
    try
    {
        input.load(file_name);

        setCurrentFile(file_name);
        return true;
    }
    catch(const std::exception& e)  // Todo
    {
        QMessageBox::warning(this, "", QString("Failed to load file: ") + e.what());  // Todo: Detailed error message
        return false;
    }
}

bool MainWindow::saveFile(const QString &file_name)
{
    try
    {
        input.meta_version = QGuiApplication::applicationVersion().toStdString();
        input.save(file_name);

        setCurrentFile(file_name);
        return true;
    }
    catch(...)  // Todo
    {
        QMessageBox::warning(this, "", "Failed to save file");  // Todo: Detailed error message
        return false;
    }
}

