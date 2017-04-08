#pragma once
#include <QtWidgets>
#include <functional>

class TaskState: public QObject
{
    Q_OBJECT

public:
    TaskState();

    bool isCanceled() const;
    void setProgress(int value);
    void cancel();

signals:
    void progressChanged(int value);

private:
    bool canceled;
};

typedef std::function<void(TaskState&)> TaskFunction;

class ProgressDialog: public QDialog
{
public:
    ProgressDialog(QWidget* parent);
    void closeEvent(QCloseEvent *event) override;
    void addTask(const QString& name, TaskFunction task);
    virtual int exec() override;

private:
    QVBoxLayout* vbox;
    std::vector<QProgressBar*> pbars;
    std::vector<TaskFunction> tasks;
};
