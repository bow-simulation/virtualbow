#include "ProgressDialog.hpp"
#include <thread>

TaskState::TaskState(QProgressBar* pbar, bool& canceled)
    : pbar(pbar), canceled(canceled)
{

}

void TaskState::setProgress(unsigned value)
{
    pbar->setValue(value);
}

bool TaskState::isCanceled() const
{
    return canceled;
}

ProgressDialog::ProgressDialog(QWidget* parent)
    : QDialog(parent),
      vbox(new QVBoxLayout)
{
    this->setLayout(vbox);
    this->setWindowTitle("Simulation Progress");
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);

    auto btbox = new QDialogButtonBox(QDialogButtonBox::Cancel);
    vbox->addSpacing(8);   // Todo: Magic number
    vbox->addWidget(btbox);
    QObject::connect(btbox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void ProgressDialog::addTask(const QString& name, TaskFunction task)
{
    auto pbar = new QProgressBar();
    pbar->setMinimumWidth(380);    // Todo: Magic number

    int i = vbox->count()-2;    // Insert above button box and spacing
    vbox->insertWidget(i, pbar);
    vbox->insertWidget(i, new QLabel(name));

    pbars.push_back(pbar);
    tasks.push_back(task);
}

int ProgressDialog::exec()
{
    bool canceled;

    QObject::connect(this, &QDialog::rejected, this, [&]()
    {
        canceled = true;
    });

    std::thread thread([&]()
    {
        for(size_t i = 0; i < tasks.size(); ++i)
        {
            TaskState state(pbars[i], canceled);
            tasks[i](state);
        }

        QDialog::accept();
    });

    QDialog::exec();
    thread.join();
}

