#pragma once
#include "gui/widgets/DialogBase.hpp"

class QVBoxLayout;

class GroupDialog: public DialogBase {
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
