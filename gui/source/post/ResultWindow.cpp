#include "ResultWindow.hpp"
#include "OutputWidget.hpp"
#include "pre/utils/UserSettings.hpp"
#include <QCloseEvent>
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>

ResultWindow::ResultWindow(const QString& filePath, const BowResult& data, QWidget* parent):
    QMainWindow(parent)
{
    // Main window properties
    setWindowFilePath(filePath);
    setWindowIcon(QIcon(":/icons/logo.svg"));
    setStyleSheet("QMainWindow { background-image:url(:/icons/background.png); background-position: center; background-repeat: no-repeat; }");
    menuBar()->setAutoFillBackground(true);
    resize({1000, 700});    // Initial size, overwritten by stored window geometry if present

    // Load state and geometry
    UserSettings settings;
    restoreState(settings.value("OutputWindow/state").toByteArray());
    restoreGeometry(settings.value("OutputWindow/geometry").toByteArray());

    // Try to load output data
    try {
        this->data = data;
        setCentralWidget(new OutputWidget(this->data));
    }
    catch(const std::exception& e) {
        QMessageBox::critical(this, "Error", "Failed to open result data:\n" + QString(e.what()));
    }
}

void ResultWindow::closeEvent(QCloseEvent* event) {
    // Save state and geometry
    UserSettings settings;
    settings.setValue("OutputWindow/state", saveState());
    settings.setValue("OutputWindow/geometry", saveGeometry());

    event->accept();
}
