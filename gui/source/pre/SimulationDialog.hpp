#pragma once
#include "pre/widgets/DialogBase.hpp"
#include "solver/API.hpp"

class SimulationDialog: public DialogBase {
    Q_OBJECT

public:
    SimulationDialog(QWidget* parent, const BowModel& model, Mode mode);
    const BowResult& getResult() const;

signals:
    void staticProgressChanged(int);
    void dynamicProgressChanged(int);

private:
    BowResult result;

    void closeEvent(QCloseEvent *event) override;
};
