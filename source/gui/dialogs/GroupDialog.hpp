#pragma once
#include "gui/BaseDialog.hpp"

class GroupDialog: public BaseDialog
{
    Q_OBJECT

public:
    GroupDialog(QWidget* parent, const QString& title, bool enable_reset);

    void addGroup(const QString& name);
    void addWidget(QWidget* widget);

signals:
    void reset();

private:
    QVBoxLayout* vbox1;
    QVBoxLayout* vbox2;
};
