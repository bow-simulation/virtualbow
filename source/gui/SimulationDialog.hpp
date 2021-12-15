#pragma once
#include "gui/widgets/BaseDialog.hpp"

class SimulationDialog: public BaseDialog
{
    Q_OBJECT

public:
    SimulationDialog(QWidget* parent, const QString& input, const QString& output, const QString& flag);

private:
    void closeEvent(QCloseEvent *event) override;
};
