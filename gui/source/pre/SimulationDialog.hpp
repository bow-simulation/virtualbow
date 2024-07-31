#pragma once
#include "pre/widgets/DialogBase.hpp"

class SimulationDialog: public DialogBase {
    Q_OBJECT

public:
    SimulationDialog(QWidget* parent, const QString& input, const QString& output, bool dynamic);

private:
    void closeEvent(QCloseEvent *event) override;
};
