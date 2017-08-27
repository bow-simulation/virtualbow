#include "MainWindow.hpp"
#include "ProgressDialog.hpp"
#include "input/BowEditor.hpp"
#include "output/OutputDialog.hpp"
#include "model/BowModel.hpp"
#include <thread>
#include <json.hpp>

MainWindow:: MainWindow(const QString& path)
    : input(path),
      editor(new BowEditor(input))
{
    // Actions
    auto action_new = new QAction(QIcon(":/icons/document-new"), "&New", this);
    QObject::connect(action_new, &QAction::triggered, this, &MainWindow::newFile);
    action_new->setShortcuts(QKeySequence::New);
    action_new->setMenuRole(QAction::NoRole);

    auto action_open = new QAction(QIcon(":/icons/document-open"), "&Open...", this);
    QObject::connect(action_open, &QAction::triggered, this, &MainWindow::open);
    action_open->setShortcuts(QKeySequence::Open);
    action_open->setMenuRole(QAction::NoRole);

    auto action_save = new QAction(QIcon(":/icons/document-save"), "&Save", this);
    QObject::connect(action_save, &QAction::triggered, this, &MainWindow::save);
    action_save->setShortcuts(QKeySequence::Save);
    action_save->setMenuRole(QAction::NoRole);

    auto action_save_as = new QAction(QIcon(":/icons/document-save-as"), "Save &As...", this);
    QObject::connect(action_save_as, &QAction::triggered, this, &MainWindow::saveAs);
    action_save_as->setShortcuts(QKeySequence::SaveAs);
    action_save_as->setMenuRole(QAction::NoRole);

    auto action_exit = new QAction(QIcon(":/icons/application-exit"), "&Quit", this);
    QObject::connect(action_exit, &QAction::triggered, this, &QWidget::close);
    action_exit->setShortcuts(QKeySequence::Quit);
    action_exit->setMenuRole(QAction::QuitRole);

    auto action_run_statics = new QAction(QIcon(":/icons/arrow-yellow"), "Statics...", this);
    action_run_statics->setShortcut(Qt::Key_F5);
    action_run_statics->setMenuRole(QAction::NoRole);
    QObject::connect(action_run_statics, &QAction::triggered, [&]{ runSimulation(false); });    // Todo: Use std::bind

    auto action_run_dynamics = new QAction(QIcon(":/icons/arrow-green"), "Dynamics...", this);
    action_run_dynamics->setShortcut(Qt::Key_F6);
    action_run_dynamics->setMenuRole(QAction::NoRole);
    QObject::connect(action_run_dynamics, &QAction::triggered, [&]{ runSimulation(true); });    // Todo: Use std::bind

    auto action_about = new QAction(QIcon(":/icons/dialog-information"), "&About...", this);
    connect(action_about, &QAction::triggered, this, &MainWindow::about);
    action_about->setMenuRole(QAction::AboutRole);

    // File menu
    auto menu_file = this->menuBar()->addMenu("&File");
    menu_file->addAction(action_new);
    menu_file->addAction(action_open);
    menu_file->addSeparator();
    menu_file->addAction(action_save);
    menu_file->addAction(action_save_as);
    menu_file->addSeparator();
    menu_file->addAction(action_exit);

    // Simulation menu
    auto menu_simulation = this->menuBar()->addMenu("&Simulate");
    menu_simulation->addAction(action_run_statics);
    menu_simulation->addAction(action_run_dynamics);

    // File toolbar
    auto toolbar_file = this->addToolBar("File");
    toolbar_file->addAction(action_new);
    toolbar_file->addAction(action_open);
    toolbar_file->addAction(action_save);
    toolbar_file->addAction(action_save_as);

    // Simulation toolbar
    auto toolbar_simulation = this->addToolBar("Simulate");
    toolbar_simulation->addAction(action_run_statics);
    toolbar_simulation->addAction(action_run_dynamics);

    // Help menu
    auto menu_help = this->menuBar()->addMenu("&Help");
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

#include <iostream>

void MainWindow::runSimulation(bool dynamics)
{
    using nlohmann::json;

    auto progress = [](unsigned p){
        qInfo() << p;
    };

    json output = BowModel::run_dynamic_simulation(input, progress, progress);
    std::cout << std::setw(4) << output;

    /*
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
    */
}

void MainWindow::about()
{
    QMessageBox::about(this, "About", "<h2>" + QApplication::applicationDisplayName() + " v" + QApplication::applicationVersion() + "</h2>"
                   "Bow and arrow physics simulation<br>"
                   "<a href=\"" + QApplication::organizationDomain() + "\">" + QApplication::organizationDomain() + "</a><br><br>"
                   "<small>Copyright (C) 2016 Stefan Pfeifer<br>"
                   "Distributed under the GNU General Public License v3.0</small>");
}

void MainWindow::setCurrentFile(const QString &path)
{
    current_file = path;
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
bool MainWindow::loadFile(const QString& path)
{
    try
    {
        input.load(path);

        setCurrentFile(path);
        return true;
    }
    catch(const std::exception& e)  // Todo
    {
        QMessageBox::warning(this, "", QString("Failed to load file: ") + e.what());  // Todo: Detailed error message
        return false;
    }
}

bool MainWindow::saveFile(const QString& path)
{
    try
    {
        input.meta_version = QGuiApplication::applicationVersion().toStdString();
        input.save(path);

        setCurrentFile(path);
        return true;
    }
    catch(...)  // Todo
    {
        QMessageBox::warning(this, "", "Failed to save file");  // Todo: Detailed error message
        return false;
    }
}

