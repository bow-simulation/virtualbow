#include "SimulationDialog.hpp"
#include <QVBoxLayout>
#include <QProgressBar>
#include <QMessageBox>
#include <QLabel>
#include <QProcess>
#include <QDialogButtonBox>
#include <QCoreApplication>
#include <QDir>
#include <cmath>

#include <iostream>

SimulationDialog::SimulationDialog(QWidget* parent, const QString& input, const QString& output, bool dynamic)
    : DialogBase(parent)
{
    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    this->setWindowTitle("Simulation Progress");
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);

    // Create static progress bar
    QProgressBar* progress1 = new QProgressBar();
    progress1->setMinimumWidth(350);    // Magic number
    progress1->setTextVisible(false);    // Looks bad on Windows otherwise
    vbox->addWidget(new QLabel("Statics"));
    vbox->addWidget(progress1);

    // Create dynamic progress bar
    QProgressBar* progress2 = nullptr;
    if(dynamic) {
        progress2 = new QProgressBar();
        progress2->setMinimumWidth(350);    // Magic number
		progress2->setTextVisible(false);    // Looks bad on Windows otherwise
        vbox->addWidget(new QLabel("Dynamics"));
        vbox->addWidget(progress2);
    }

    // Create buttons
    auto btbox = new QDialogButtonBox(QDialogButtonBox::Cancel);
    QObject::connect(btbox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    vbox->addSpacing(8);    // Magic number
    vbox->addWidget(btbox);

    // Create solver process
    auto process = new QProcess(this);
    process->setWorkingDirectory(QCoreApplication::applicationDirPath());
    process->setProgram(QDir(QCoreApplication::applicationDirPath()).filePath("virtualbow-slv"));
    process->setArguments({ dynamic ? "dynamic" : "static", input, output, "--progress" });

    QObject::connect(this, &QDialog::rejected, this, [=] {
        // User canceled the dialog: Terminate process and wait until it has finished
        process->terminate();
        process->waitForFinished();
    });

    QObject::connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
	this, [=](int exitCode, QProcess::ExitStatus exitStatus) {
        // Accept the dialog if the process finished successfully
        // Otherwise show its error output, if there are any
        // (there are none if the process was terminated by the user)
        if(exitStatus == QProcess::NormalExit && exitCode == 0) {
            this->accept();
        } else {
            QString error(process->readAllStandardError());
            if(!error.isEmpty()) {
                QMessageBox::critical(this, "Error", error);
                this->reject();
            }
        }
    });

    QObject::connect(process, &QProcess::readyReadStandardOutput, this, [=] {
        QString line(process->readAll());

        QStringList parts = line.split(",");
        if(parts.size() != 2) {
            return;
        }

        QStringList parts_stage = parts[0].split(":");
        if(parts_stage.size() != 2) {
            return;
        }

        QStringList parts_progress = parts[1].split(":");
        if(parts_progress.size() != 2) {
            return;
        }

        QString stage = parts_stage[1].trimmed();
        QString progress = parts_progress[1].remove("%").trimmed();
        int value = round(progress.toDouble());

        if(stage == "statics" && progress1 != nullptr) {
            progress1->setValue(value);
        }
        else if(stage == "dynamics" && progress2 != nullptr) {
            progress2->setValue(value);
        }

    });

    QObject::connect(process, &QProcess::errorOccurred, this, [=](QProcess::ProcessError error) {
        QMessageBox::critical(this, "Error", "Could not run the simulation, solver process failed to start.");
        this->reject();
    });

    process->start();
}

void SimulationDialog::closeEvent(QCloseEvent *event) {
    this->reject();
}
