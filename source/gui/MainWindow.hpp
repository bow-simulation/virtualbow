#pragma once
#include "../model/InputData.hpp"
#include "BowEditor.hpp"
#include "SettingsDialog.hpp"

#include <QtWidgets>

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(): editor(new BowEditor(data))
    {
        // Actions
        QAction* action_new = new QAction(QIcon(":/document-new"), "&New", this);
        //QAction *action_new = new QAction(QIcon(":/document-new"), "&New", this);
        action_new->setShortcuts(QKeySequence::New);
        QObject::connect(action_new, &QAction::triggered, this, &MainWindow::newFile);

        QAction* action_open = new QAction(QIcon(":/document-open"), "&Open...", this);
        action_open->setShortcuts(QKeySequence::Open);
        QObject::connect(action_open, &QAction::triggered, this, &MainWindow::open);

        QAction* action_save = new QAction(QIcon(":/document-save"), "&Save", this);
        action_save->setShortcuts(QKeySequence::Save);
        QObject::connect(action_save, &QAction::triggered, this, &MainWindow::save);

        QAction* action_save_as = new QAction(QIcon(":/document-save-as"), "Save &As...", this);
        action_save_as->setShortcuts(QKeySequence::SaveAs);
        QObject::connect(action_save_as, &QAction::triggered, this, &MainWindow::saveAs);

        QAction* action_exit = new QAction(QIcon(":/application-exit"), "&Quit", this);
        action_exit->setShortcuts(QKeySequence::Quit);
        QObject::connect(action_exit, &QAction::triggered, this, &QWidget::close);

        QAction* action_run_statics = new QAction(QIcon(":/arrow-right"), "Simulate statics...", this);
        // action_run_statics->setShortcuts(QKeySequence::Quit);
        // QObject::connect ...

        QAction* action_run_dynamics = new QAction(QIcon(":/arrow-right-double"), "Simulate dynamics...", this);
        // action_run_dynmics->setShortcuts(QKeySequence::Quit);
        // QObject::connect ...

        QAction* action_settings = new QAction("Settings...", this);
        connect(action_settings, &QAction::triggered, this, &MainWindow::settings);

        QAction* action_about = new QAction(QIcon(":/dialog-information"), "&About...", this);
        connect(action_about, &QAction::triggered, this, &MainWindow::about);

        // File menu
        QMenu* menu_file = this->menuBar()->addMenu("&File");
        menu_file->addAction(action_new);
        menu_file->addAction(action_open);
        menu_file->addAction(action_save);
        menu_file->addAction(action_save_as);
        menu_file->addSeparator();
        menu_file->addAction(action_exit);

        // Simulation menu
        QMenu* menu_simulation = this->menuBar()->addMenu("&Simulation");
        menu_simulation->addAction(action_run_statics);
        menu_simulation->addAction(action_run_dynamics);
        menu_simulation->addSeparator();
        menu_simulation->addAction(action_settings);

        // File toolbar
        QToolBar* toolbar_file = this->addToolBar("File");
        toolbar_file->addAction(action_new);
        toolbar_file->addAction(action_open);
        toolbar_file->addAction(action_save);
        toolbar_file->addAction(action_save_as);

        // Simulation toolbar
        QToolBar* toolbar_simulation = this->addToolBar("Simulation");
        toolbar_simulation->addAction(action_run_statics);
        toolbar_simulation->addAction(action_run_dynamics);

        // Help menu
        QMenu* menu_help = this->menuBar()->addMenu("&Help");
        menu_help->addAction(action_about);

        this->setWindowIcon(QIcon(":/logo"));
        this->setCentralWidget(editor);
        setCurrentFile(QString());
    }

private slots:
    void closeEvent(QCloseEvent *event) override
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

    void newFile()
    {
        if(!optionalSave())
            return;

        data = InputData();
        setCurrentFile(QString());
    }

    void open()
    {
        if(!optionalSave())
            return;

        QFileDialog dialog(this);
        dialog.setAcceptMode(QFileDialog::AcceptOpen);
        dialog.setNameFilter("Bow Files (*.bow)");

        if(dialog.exec() == QDialog::Accepted)
            loadFile(dialog.selectedFiles().first());
    }

    bool save()
    {
        if(current_file.isEmpty())
            return saveAs();

        return saveFile(current_file);
    }

    bool saveAs()
    {

        QFileDialog dialog(this);
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setNameFilter("Bow Files (*.bow)");
        dialog.setDefaultSuffix("bow");

        if(dialog.exec() == QDialog::Accepted)
            return saveFile(dialog.selectedFiles().first());

        return false;
    }

    // Todo: settings and about dialogs as lambdas in constructor?
    void settings()
    {
        SettingsDialog dialog(this, data);
        dialog.exec();
    }

    void about()
    {
        QMessageBox msgbox(this);
        msgbox.setIconPixmap(QPixmap(":/logo"));
        msgbox.setWindowTitle("About");
        msgbox.setText("<h2>" + QGuiApplication::applicationDisplayName() + "</h2>"
                       "<h4>Version " + QGuiApplication::applicationVersion() + "</h4>"
                       "Bow and arrow physics simulation<br>"
                       "<a href=\"" + QGuiApplication::organizationDomain() + "\">" + QGuiApplication::organizationDomain() + "</a><br><br>"
                       "Copyright (C) 2016 Stefan Pfeifer<br>"
                       "Distributed under the GNU General Public License v3.0");
        msgbox.exec();
    }

private:
    InputData data;
    BowEditor* editor;
    QString current_file;

    void setCurrentFile(const QString &file_name)
    {
        current_file = file_name;
        setWindowModified(false);
        setWindowFilePath(current_file.isEmpty() ? "untitled.bow" : current_file);
    }

    bool optionalSave()    // true: Discard, false: Cancel
    {
        if(!data.is_modified())
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
    bool loadFile(const QString &file_name)
    {
        try
        {
            data = InputData();
            data.load(file_name.toStdString());
            data.set_modified(false);

            setCurrentFile(file_name);
            return true;
        }
        catch(...)  // Todo
        {
            QMessageBox::warning(this, "", "Failed to load file");  // Todo: Detailed error message
            return false;
        }
    }

    bool saveFile(const QString &file_name)
    {
        try
        {
            data.meta_version = QGuiApplication::applicationVersion().toStdString();
            data.save(file_name.toStdString());
            data.set_modified(false);

            setCurrentFile(file_name);
            return true;
        }
        catch(...)  // Todo
        {
            QMessageBox::warning(this, "", "Failed to save file");  // Todo: Detailed error message
            return false;
        }
    }
};
