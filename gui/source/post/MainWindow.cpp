#include "MainWindow.hpp"
#include "OutputWidget.hpp"
#include "pre/HelpMenu.hpp"
#include "pre/RecentFilesMenu.hpp"
#include "pre/UnitDialog.hpp"
#include "pre/utils/UserSettings.hpp"

#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow()
    : menu_open_recent(new RecentFilesMenu(this))
{
    // Actions
    action_open = new QAction(QIcon(":/icons/document-open.svg"), "&Open...", this);
    action_open->setShortcuts(QKeySequence::Open);
    action_open->setMenuRole(QAction::NoRole);
    QObject::connect(action_open, &QAction::triggered, this, &MainWindow::open);

    action_save_as = new QAction(QIcon(":/icons/document-save-as.svg"), "Save &As...", this);
    action_save_as->setShortcuts(QKeySequence::SaveAs);
    action_save_as->setMenuRole(QAction::NoRole);
    QObject::connect(action_save_as, &QAction::triggered, this, &MainWindow::saveAs);

    action_quit = new QAction("&Quit", this);
    action_quit->setShortcuts(QKeySequence::Quit);
    action_quit->setMenuRole(QAction::QuitRole);
    QObject::connect(action_quit, &QAction::triggered, this, &QWidget::close);

    action_set_units = new QAction("&Units...", this);
    QObject::connect(action_set_units, &QAction::triggered, this, [&]{
        UnitDialog dialog(this);
        dialog.exec();
    });
    action_set_units->setMenuRole(QAction::NoRole);

    // File menu
    auto menu_file = this->menuBar()->addMenu("&File");
    menu_file->addAction(action_open);
    menu_file->addMenu(menu_open_recent);
    menu_file->addSeparator();
    menu_file->addAction(action_save_as);
    menu_file->addSeparator();
    menu_file->addAction(action_quit);

    // Recent files menu
    QObject::connect(menu_open_recent, &RecentFilesMenu::openRecent, this, &MainWindow::loadFile);
    QObject::connect(menu_file, &QMenu::aboutToShow, [&]{ menu_open_recent->updateActions(); });

    // Options menu
    auto menu_preferences = this->menuBar()->addMenu("&Options");
    menu_preferences->addAction(action_set_units);

    // Help menu
    this->menuBar()->addMenu(new HelpMenu(this));

    // Main window
    this->setWindowIcon(QIcon(":/icons/logo.svg"));
    this->setStyleSheet("QMainWindow { background-image:url(:/icons/background.png); background-position: center; background-repeat: no-repeat; }");
    this->menuBar()->setAutoFillBackground(true);
    this->resize(INITIAL_SIZE);

    // Load geometry and state
    UserSettings settings;
    restoreState(settings.value("OutputWindow/state").toByteArray());
    restoreGeometry(settings.value("OutputWindow/geometry").toByteArray());

    // Load units settings
    Quantities::loadFromSettings(settings);

    // Disable saving until a file has been loaded
    action_save_as->setEnabled(false);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // Save state and geometry
    UserSettings settings;
    settings.setValue("OutputWindow/state", saveState());
    settings.setValue("OutputWindow/geometry", saveGeometry());
}

void MainWindow::loadFile(const QString& path) {
    try {
        data = OutputData2(path.toLocal8Bit().toStdString());    // toLocal8Bit() for Windows, since toStdString() would convert to UTF8
        this->setCentralWidget(new OutputWidget(data));
        this->setWindowFilePath(path);
        action_save_as->setEnabled(true);
        menu_open_recent->addPath(path);
    }
    catch(const std::exception& e) {
        QMessageBox::critical(this, "Error", "Failed to open " + path + ":\n" + e.what());
    }
}

void MainWindow::saveFile(const QString &path) {
    try {
        auto widget = dynamic_cast<OutputWidget*>(this->centralWidget());
        widget->getData().save(path.toLocal8Bit().toStdString());    // toLocal8Bit() for Windows, since toStdString() would convert to UTF8
        this->setWindowFilePath(path);
    }
    catch(const std::exception& e) {
        QMessageBox::critical(this, "Error", "Failed to save " + path + ":\n" + e.what());
    }
}

void MainWindow::open() {
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter("Result Files (*.res)");

    if(dialog.exec() == QDialog::Accepted) {
        loadFile(dialog.selectedFiles().first());
    }
}

void MainWindow::saveAs() {
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter("Result Files (*.res)");
    dialog.setDefaultSuffix("res");
    dialog.selectFile(this->windowFilePath().isEmpty() ? DEFAULT_FILENAME : this->windowFilePath());

    if(dialog.exec() == QDialog::Accepted) {
        saveFile(dialog.selectedFiles().first());
    }
}
