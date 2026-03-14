#include "SimulationDialog.hpp"
#include <QtConcurrent/QtConcurrent>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QMessageBox>
#include <QCloseEvent>
#include <QLabel>
#include <QProcess>
#include <QDialogButtonBox>
#include <QCoreApplication>
#include <QDir>
#include <cmath>

SimulationDialog::SimulationDialog(QWidget* parent, const BowModel& model, Mode mode)
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
    vbox->addSpacing(8);
    vbox->addWidget(btbox);

    // Create future watcher and make cancel button cancel the future
    auto watcher = new QFutureWatcher<QString>(this);
    QObject::connect(btbox, &QDialogButtonBox::rejected, this, [=] {
        watcher->future().cancel();
    });

    // When the task has finished, accept/reject the dialog or show an error messagebox as needed
    QObject::connect(watcher, &QFutureWatcher<QString>::finished, this, [=, this] {
        // Future was canceled: Reject the dialog and do nothing else
        if(watcher->future().isCanceled()) {
            reject();
        }
        else {
            // Otherwise have a look at the result of the future, which is an optional error string.
            // No error: Accept the dialog, Error: Reject the dialog and show the error.
            QString error = watcher->future().result();
            if(error.isEmpty()) {
                accept();
            }
            else {
                QMessageBox::critical(this, "Error", error);
                reject();
            }
        }
    });

    // Run the simulation task in a separate thread and return a string with the error message on exceptions
    // Communicate static and dynamic progress by custom signals since QPromise only handles one progress value
    QFuture<QString> future = QtConcurrent::run([&, mode](QPromise<QString>& promise) {
        try {
            result = simulate_model(model, mode, [&](Mode stage, double progress) {
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

const BowResult& SimulationDialog::getResult() const {
    return result;
}

void SimulationDialog::closeEvent(QCloseEvent* event) {
    //event->accept();
    //this->reject();
}
