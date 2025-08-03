#pragma once
#include "pre/widgets/DialogBase.hpp"
#include "solver/API.hpp"

class SimulationDialog: public DialogBase {
    Q_OBJECT

public:
    SimulationDialog(QWidget* parent, const QString& modelFile, const QString& resultFile, Mode mode);

signals:
    void staticProgressChanged(int);
    void dynamicProgressChanged(int);

private:
    void closeEvent(QCloseEvent *event) override;
};
