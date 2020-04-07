#include "SimulationDialog.hpp"
#include <thread>

SimulationDialog::SimulationDialog(QWidget* parent, const QString& input, const QString& output, const QString& flag)
    : BaseDialog(parent)
{
    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    this->setWindowTitle("Simulation Progress");
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);

    // Create static progress bar
    QProgressBar* progress1 = new QProgressBar();
    progress1->setMinimumWidth(350);    // Magic number
    vbox->addWidget(new QLabel("Statics"));
    vbox->addWidget(progress1);

    // Create dynamic progress bar
    QProgressBar* progress2 = nullptr;
    if(flag == "--dynamic") {
        progress2 = new QProgressBar();
        progress2->setMinimumWidth(350);    // Magic number
        vbox->addWidget(new QLabel("Dynamics"));
        vbox->addWidget(progress2);
    }

    // Create buttons
    auto btbox = new QDialogButtonBox(QDialogButtonBox::Cancel);
    QObject::connect(btbox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    vbox->addSpacing(8);    // Magic number
    vbox->addWidget(btbox);

    // Create solver process
    QProcess *process = new QProcess(this);
    process->setWorkingDirectory(QCoreApplication::applicationDirPath());
    process->setProgram("virtualbow-slv");
    process->setArguments({ input, output, flag, "--progress" });

    QObject::connect(this, &QDialog::rejected, this, [=] {
        // User canceled the dialog: Terminate process and wait until it has finished
        process->terminate();
        process->waitForFinished();
    });

    QObject::connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int exitCode, QProcess::ExitStatus exitStatus) {
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

    QObject::connect(process, &QProcess::readyRead, this, [=] {
        QString line(process->readAll());
        if(progress1 != nullptr) {
            progress1->setValue(line.section("\t", 0, 0).toInt());
        }
        if(progress2 != nullptr) {
            progress2->setValue(line.section("\t", 1, 1).toInt());
        }
    });

    process->start();
}

void SimulationDialog::closeEvent(QCloseEvent *event)
{
    this->reject();
}
