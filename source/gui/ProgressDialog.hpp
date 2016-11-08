#pragma once
#include <QtWidgets>
#include <functional>

class TaskState
{
public:
    TaskState(QProgressBar* pbar, bool& canceled);
    void setProgress(unsigned value);
    bool isCanceled() const;

private:
    QProgressBar* pbar;
    bool& canceled;
};

typedef std::function<void(TaskState&)> TaskFunction;

class ProgressDialog: public QDialog
{
public:
    ProgressDialog(QWidget* parent);
    void addTask(const QString& name, TaskFunction task);
    virtual int exec() override;

private:
    QVBoxLayout* vbox;
    std::vector<QProgressBar*> pbars;
    std::vector<TaskFunction> tasks;
};
