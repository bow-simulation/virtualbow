#pragma once
#include "gui/widgets/DialogBase.hpp"

class SimulationDialog: public DialogBase
{
    Q_OBJECT

public:
    SimulationDialog(QWidget* parent, const QString& input, const QString& output, const QString& flag);

private:
    void closeEvent(QCloseEvent *event) override;
};
