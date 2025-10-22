#include "ResultWindow.hpp"
#include "OutputWidget.hpp"
#include "pre/utils/UserSettings.hpp"
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>

ResultWindow::ResultWindow() {
    // Main window properties
    this->setWindowTitle("Results");
    this->setWindowIcon(QIcon(":/icons/logo.svg"));
    this->setStyleSheet("QMainWindow { background-image:url(:/icons/background.png); background-position: center; background-repeat: no-repeat; }");
    this->menuBar()->setAutoFillBackground(true);
    this->resize({1000, 700});    // Initial size, overwritten by stored window geometry if present

    // Load geometry and state
    UserSettings settings;
    restoreState(settings.value("OutputWindow/state").toByteArray());
    restoreGeometry(settings.value("OutputWindow/geometry").toByteArray());
}

void ResultWindow::closeEvent(QCloseEvent *event) {
    // Save state and geometry
    UserSettings settings;
    settings.setValue("OutputWindow/state", saveState());
    settings.setValue("OutputWindow/geometry", saveGeometry());
}

void ResultWindow::load(const BowResult& data) {
    try {
        this->data = data;
        this->setCentralWidget(new OutputWidget(this->data));
    }
    catch(const std::exception& e) {
        QMessageBox::critical(this, "Error", "Failed to open result data:\n" + QString(e.what()));
    }
}
