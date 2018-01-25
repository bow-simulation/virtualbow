#pragma once
#include "BaseDialog.hpp"

class ProgressDialog: public BaseDialog
{
    Q_OBJECT

public:
    ProgressDialog(QWidget* parent);
    // virtual void closeEvent(QCloseEvent *event) override;
    virtual void reject() override;
    bool isCanceled() const;
    void addProgressBar(const QString& name);

public slots:
    void setProgress(int task, int value);

private:
    QVBoxLayout* vbox;
    std::vector<QProgressBar*> pbars;
    bool canceled;
};
