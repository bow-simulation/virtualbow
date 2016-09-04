#pragma once
#include "BowEditor.hpp"

#include <QtWidgets>

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(): editor(new BowEditor(document))
    {
        // Actions
        QAction *action_new = new QAction(QIcon::fromTheme("document-new"), "&New", this);
        action_new->setShortcuts(QKeySequence::New);
        connect(action_new, &QAction::triggered, this, &MainWindow::newFile);

        QAction *action_open = new QAction(QIcon::fromTheme("document-open"), "&Open...", this);
        action_open->setShortcuts(QKeySequence::Open);
        connect(action_open, &QAction::triggered, this, &MainWindow::open);

        QAction *action_save = new QAction(QIcon::fromTheme("document-save"), "&Save", this);
        action_save->setShortcuts(QKeySequence::Save);
        connect(action_save, &QAction::triggered, this, &MainWindow::save);

        QAction *action_save_as = new QAction(QIcon::fromTheme("document-save-as"), "Save &As...", this);
        action_save_as->setShortcuts(QKeySequence::SaveAs);
        connect(action_save_as, &QAction::triggered, this, &MainWindow::saveAs);

        QAction *action_exit = new QAction(QIcon::fromTheme("application-exit"), "&Quit", this);
        action_exit->setShortcuts(QKeySequence::Quit);
        connect(action_exit, &QAction::triggered, this, &QWidget::close);

        QAction *action_run_statics = new QAction("Statics...", this);
        //action_run_statics->setShortcuts(QKeySequence::Quit);
        // connect ...

        QAction *action_run_dynamics = new QAction("Dynamics...", this);
        //action_run_dynmics->setShortcuts(QKeySequence::Quit);
        // connect ...

        QAction *action_about = new QAction(QIcon::fromTheme("dialog-information"), "&About...", this);
        connect(action_about, &QAction::triggered, this, &MainWindow::about);

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

        this->setCentralWidget(editor);
        setCurrentFile(QString());
    }

private slots:
    void closeEvent(QCloseEvent *event) override
    {
        //this->setFocus();   // Make all views lose their focus so they propagate their changes to the document

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

        document.setData(InputData());
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

    void about()
    {
        QMessageBox::information(this, "About", "<h2>" + QCoreApplication::applicationName() + "</h2>"
                                                "<h4>Version " + QCoreApplication::applicationVersion() + "</h4>"
                                                "Bow and arrow physics simulation<br>"
                                                "<a href=\"http://stfnp.bitbucket.org\">http://stfnp.bitbucket.org</a><br><br>"
                                                "Copyright (C) 2016 Stefan Pfeifer<br>"
                                                "Distributed under the GNU General Public License v3.0");
    }

private:
    Document document;
    BowEditor* editor;
    QString current_file;

    void setCurrentFile(const QString &file_name)
    {
        current_file = file_name;
        setWindowModified(false);
        setWindowFilePath(current_file.isEmpty() ? "untitled.txt" : current_file);
    }

    bool optionalSave()    // true: Discard, false: Cancel
    {
        if(!document.isModified())
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
            InputData data;
            data.load(file_name.toStdString());

            document.setData(data);                 // Todo: Avoid this copying? Access by reference?
            document.setModified(false);
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
            document.getData().save(file_name.toStdString());
            document.setModified(false);
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
