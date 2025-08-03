#include "SimulationDialog.hpp"
#include <QtConcurrent/QtConcurrent>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QMessageBox>
#include <QLabel>
#include <QProcess>
#include <QDialogButtonBox>
#include <QCoreApplication>
#include <QDir>
#include <cmath>

SimulationDialog::SimulationDialog(QWidget* parent, const QString& modelFile, const QString& resultFile, Mode mode)
    : DialogBase(parent)
{
    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    this->setWindowTitle("Simulation Progress");
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);

    // Create static progress bar
    QProgressBar* staticProgress = new QProgressBar();
    staticProgress->setMinimumWidth(350);
    staticProgress->setTextVisible(false);    // Looks bad on Windows otherwise
    vbox->addWidget(new QLabel("Statics"));
    vbox->addWidget(staticProgress);
    QObject::connect(this, &SimulationDialog::staticProgressChanged, staticProgress, &QProgressBar::setValue);    // Update progress value

    // Create dynamic progress bar
    QProgressBar* dynamicProgress = nullptr;
    if(mode == Mode::Dynamic) {
        dynamicProgress = new QProgressBar();
        dynamicProgress->setMinimumWidth(350);
        dynamicProgress->setTextVisible(false);    // Looks bad on Windows otherwise
        vbox->addWidget(new QLabel("Dynamics"));
        vbox->addWidget(dynamicProgress);
        QObject::connect(this, &SimulationDialog::dynamicProgressChanged, dynamicProgress, &QProgressBar::setValue);    // Update progress value
    }

    // Create cancel button
    auto btbox = new QDialogButtonBox(QDialogButtonBox::Cancel);
    QObject::connect(btbox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    vbox->addSpacing(8);
    vbox->addWidget(btbox);

    // Run simulation as an asynchronous task

    auto watcher = new QFutureWatcher<QString>(this);

    // Cancel the task when the dialog was rejected
    QObject::connect(this, &QDialog::rejected, this, [=] {
        watcher->future().cancel();
    });

    // When the task has finished, accept the dialog if no error occurred or show a messagebow and reject
    QObject::connect(watcher, &QFutureWatcher<void>::finished, this, [=, this] {
        QString error = watcher->future().result();
        if(error.isEmpty()) {
            accept();
        }
        else {
            QMessageBox::critical(this, "Error", error);
            reject();
        }
    });

    // Run the simulation task in a separate thread and return a string with the error message on exceptions
    // Communicate static and dynamic progress by custom signals since QPromise only handles one progress value
    QFuture<QString> future = QtConcurrent::run([&, mode](QPromise<QString>& promise) {
        try {
            BowModel model = load_model(modelFile.toStdString(), false);
            BowResult result = simulate_model(model, mode, [&](Mode stage, double progress) {
                switch(stage) {
                case Mode::Static:
                    emit staticProgressChanged(progress);
                    break;
                case Mode::Dynamic:
                    emit dynamicProgressChanged(progress);
                    break;
                }

                return !promise.isCanceled();    // Continue the simulation as long as the future has not been canceled
            });

            qInfo() << "Simulation finished";
            save_result(result, resultFile.toStdString());
            qInfo() << "Saving finished";
        }
        catch(const SolverException& e) {
            if(!promise.isCanceled()) {
                promise.addResult(e.what());   // The solver api throws an exception on cancellation, but we only care about exceptions in the no-cancel case.
                return;
            }
        }

        promise.addResult(QString());
    });

    watcher->setFuture(future);
}

void SimulationDialog::closeEvent(QCloseEvent *event) {
    this->reject();
}
