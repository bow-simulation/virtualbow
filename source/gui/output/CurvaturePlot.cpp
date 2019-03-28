#include "CurvaturePlot.hpp"
#include "gui/input/views/LayerColors.hpp"

CurvaturePlot::CurvaturePlot(const InputData& input, const LimbProperties& limb, const BowStates& states)
    : input(input),
      limb(limb),
      states(states)
{
    this->xAxis->setLabel("Arc length [m]");
    this->yAxis->setLabel("Curvature [1/m]");
    this->setupTopLegend();

    this->addGraph();
    this->graph(0)->setName("Curvature");

    setAxesRanges();
}

void CurvaturePlot::setStateIndex(int index)
{
    VectorXd curvature(states.angle_limb[index].size());
    for(int j = 1; j<states.angle_limb[index].size(); j++) {
        double x0 = states.x_pos_limb[index][j-1];
        double y0 = states.y_pos_limb[index][j-1];
        double a0 = states.angle_limb[index][j-1];
        double x1 = states.x_pos_limb[index][j];
        double y1 = states.y_pos_limb[index][j];
        double a1 = states.angle_limb[index][j];
        curvature[j] = -sin(a1-a0)/sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
    }
    curvature[0] = curvature[1];
    this->graph(0)->setData(states.x_pos_limb[index], curvature);

    this->replot();
}

void CurvaturePlot::setAxesRanges()
{
    QCPRange x_range(limb.length.minCoeff(), limb.length.maxCoeff());
    QCPRange y_range(0.0, 0.0);

    for(size_t i = 0; i < states.time.size(); ++i)
    {
        for(size_t j = 1; j < states.angle_limb[i].size(); j++) {
            double x0 = states.x_pos_limb[i][j-1];
            double y0 = states.y_pos_limb[i][j-1];
            double a0 = states.angle_limb[i][j-1];
            double x1 = states.x_pos_limb[i][j];
            double y1 = states.y_pos_limb[i][j];
            double a1 = states.angle_limb[i][j];
            y_range.expand(-sin(a1-a0)/sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0)));
        }
    }

    this->setAxesLimits(x_range, y_range);
}
