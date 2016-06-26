#pragma once
#include <QtWidgets>

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    MainWindow()
    {
        this->resize(800, 600);

        // Actions
        QAction *action_new = new QAction(QIcon::fromTheme("document-new"), "&New", this);
        action_new->setShortcuts(QKeySequence::New);
        //action_new->setToolTip("Create new file");
        //connect(action_new, &QAction::triggered, this, &MainWindow::newFile);

        QAction *action_open = new QAction(QIcon::fromTheme("document-open"), "&Open...", this);
        action_open->setShortcuts(QKeySequence::Open);
        //action_open->setToolTip("Open an existing file");
        //connect(action_open, &QAction::triggered, this, &MainWindow::open);

        QAction *action_save = new QAction(QIcon::fromTheme("document-save"), "&Save", this);
        action_save->setShortcuts(QKeySequence::Save);
        //action_save->setToolTip("Save file");
        //connect(action_save, &QAction::triggered, this, &MainWindow::save);

        QAction *action_save_as = new QAction(QIcon::fromTheme("document-save-as"), "Save &As...", this);
        action_save_as->setShortcuts(QKeySequence::SaveAs);
        //action_save_as->setToolTip("Save file under a new name");
        // connect ...

        QAction *action_exit = new QAction(QIcon::fromTheme("application-exit"), "&Quit", this);
        action_exit->setShortcuts(QKeySequence::Quit);
        // connect ...

        QAction *action_run_statics = new QAction("Statics...", this);
        //action_run_statics->setShortcuts(QKeySequence::Quit);
        // connect ...

        QAction *action_run_dynamics = new QAction("Dynamics...", this);
        //action_run_dynmics->setShortcuts(QKeySequence::Quit);
        // connect ...

        QAction *action_about = new QAction(QIcon::fromTheme("dialog-information"), "&About...", this);
        //action_about->setStatusTip("Show the application's About box");

        // File menu
        QMenu *menu_file = this->menuBar()->addMenu("&File");
        menu_file->addAction(action_new);
        menu_file->addAction(action_open);
        menu_file->addAction(action_save);
        menu_file->addAction(action_save_as);
        menu_file->addSeparator();
        menu_file->addAction(action_exit);

        // File toolbar
        QToolBar *toolbar_file = this->addToolBar("File");
        toolbar_file->addAction(action_new);
        toolbar_file->addAction(action_open);
        toolbar_file->addAction(action_save);
        toolbar_file->addAction(action_save_as);

        // Simulation menu
        QMenu *menu_run = this->menuBar()->addMenu("&Simulate");
        menu_run->addAction(action_run_statics);
        menu_run->addAction(action_run_dynamics);

        // Help menu
        QMenu *menu_help = this->menuBar()->addMenu("&Help");
        menu_help->addAction(action_about);

    }
};
