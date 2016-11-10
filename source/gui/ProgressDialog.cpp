#include "ProgressDialog.hpp"
#include <thread>

TaskState::TaskState(): canceled(false)
{

}

void TaskState::cancel()
{
    canceled = true;
}

bool TaskState::isCanceled() const
{
    return canceled;
}

void TaskState::setProgress(int value)
{
    emit progressChanged(value);
}

ProgressDialog::ProgressDialog(QWidget* parent)
    : QDialog(parent),
      vbox(new QVBoxLayout)
{
    this->setLayout(vbox);
    this->setWindowTitle("Simulation Progress");
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);

    auto btbox = new QDialogButtonBox(QDialogButtonBox::Cancel);
    vbox->addWidget(btbox);
    QObject::connect(btbox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void ProgressDialog::closeEvent(QCloseEvent *event)
{
    this->reject();
}

void ProgressDialog::addTask(const QString& name, TaskFunction task)
{
    auto pbar = new QProgressBar();
    pbar->setMinimumWidth(380);    // Todo: Magic number

    int i = vbox->count()-1;    // Insert above button box
    vbox->insertWidget(i, pbar);
    vbox->insertWidget(i, new QLabel(name));

    pbars.push_back(pbar);
    tasks.push_back(task);
}

int ProgressDialog::exec()
{
    // On calling slots from different threads:
    // http://stackoverflow.com/questions/1144240/qt-how-to-call-slot-from-custom-c-code-running-in-a-different-thread
    std::thread thread([&]()
    {
        for(size_t i = 0; i < tasks.size(); ++i)
        {
            TaskState state;
            QObject::connect(&state, &TaskState::progressChanged, pbars[i], &QProgressBar::setValue, Qt::QueuedConnection);
            //QObject::connect(this, &QDialog::rejected, &state, &TaskState::cancel, Qt::QueuedConnection); // Todo: Why doesn't this shit work?
            QObject::connect(this, &QDialog::rejected, [&](){ state.cancel(); });

            tasks[i](state);

            if(state.isCanceled())
                return;
        }

        QMetaObject::invokeMethod(this, "accept", Qt::QueuedConnection);
    });

    QDialog::exec();
    thread.join();
}
