#include "ProgressDialog.hpp"
#include <thread>

TaskState::TaskState()
    : canceled(false)
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
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);

    auto btbox = new QDialogButtonBox(QDialogButtonBox::Cancel);
    vbox->addSpacing(8);    // Todo: Magic number
    vbox->addWidget(btbox);
    QObject::connect(btbox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void ProgressDialog::closeEvent(QCloseEvent *event)
{
    this->reject();
}

void ProgressDialog::addTask(const QString& name, TaskFunction task)
{
    // Create new progress bar
    auto pbar = new QProgressBar();
    pbar->setTextVisible(false);
    pbar->setMinimumWidth(300);    // Todo: Magic number

    // Insert bar and label above button box
    int i = vbox->count()-2;
    vbox->insertWidget(i, pbar);
    vbox->insertWidget(i, new QLabel(name));

    pbars.push_back(pbar);
    tasks.push_back(task);
}


int ProgressDialog::exec()
{
    // Food for thought...
    // Calling slots from different threads:
    // http://stackoverflow.com/questions/1144240/qt-how-to-call-slot-from-custom-c-code-running-in-a-different-thread
    // Propagating exceptions across thread boundaries:
    // http://stackoverflow.com/questions/233127/how-can-i-propagate-exceptions-between-threads

    std::exception_ptr exception = nullptr;
    std::thread thread([&]()
    {
        try
        {
            for(size_t i = 0; i < tasks.size(); ++i)
            {
                TaskState state;
                QObject::connect(&state, &TaskState::progressChanged, pbars[i], &QProgressBar::setValue, Qt::QueuedConnection);
                QObject::connect(this, &QDialog::rejected, [&](){ state.cancel(); });
                //QObject::connect(this, &QDialog::rejected, &state, &TaskState::cancel, Qt::QueuedConnection); // Todo: Why doesn't this shit work?

                tasks[i](state);

                if(state.isCanceled())
                    return;
            }

            QMetaObject::invokeMethod(this, "accept", Qt::QueuedConnection);
        }
        catch(...)
        {
            exception = std::current_exception();
            QMetaObject::invokeMethod(this, "reject", Qt::QueuedConnection);
        }
    });

    int result = QDialog::exec();

    thread.join();
    if(exception)
    {
        std::rethrow_exception(exception);
    }

    return result;
}
