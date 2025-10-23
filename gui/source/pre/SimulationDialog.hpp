#pragma once
#include "pre/widgets/DialogBase.hpp"
#include "solver/API.hpp"

class SimulationDialog: public DialogBase {
    Q_OBJECT

public:
    SimulationDialog(QWidget* parent, const BowModel& model, BowResult& result, Mode mode);

signals:
    void staticProgressChanged(int);
    void dynamicProgressChanged(int);

private:
    void closeEvent(QCloseEvent *event) override;
};
