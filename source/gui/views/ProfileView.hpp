#pragma once
#include "gui/PlotWidget.hpp"
#include "solver/numerics/Eigen.hpp"
#include "solver/numerics/Series.hpp"

class ProfileView: public PlotWidget {
public:
    ProfileView();
    void setData(const MatrixXd& profile);
    void setSelection(const QVector<int>& selection);
private:
    MatrixXd data;
    QVector<int> selection;

    QVector<QCPCurve*> curves;
    QVector<QCPCurve*> nodes;

    void updatePlot();
    void resetPlot();
    void updateHighlights();
};
