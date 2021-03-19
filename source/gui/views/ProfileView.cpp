#include "ProfileView.hpp"
#include "solver/model/ProfileCurve.hpp"
#include "solver/numerics/Linspace.hpp"

ProfileView::ProfileView(const UnitGroup& unit)
    : unit(unit)
{
    this->setAspectPolicy(PlotWidget::SCALE_Y);

    // Line
    curve0 = new QCPCurve(this->xAxis, this->yAxis);
    curve0->setPen({Qt::blue, 2});
    curve0->setScatterSkip(0);

    // Unselected nodes
    curve1 = new QCPCurve(this->xAxis, this->yAxis);
    curve1->setScatterStyle({QCPScatterStyle::ssSquare, Qt::blue, 8});
    curve1->setLineStyle(QCPCurve::lsNone);
    curve1->setScatterSkip(0);

    // Selected nodes
    curve2 = new QCPCurve(this->xAxis, this->yAxis);
    curve2->setScatterStyle({QCPScatterStyle::ssSquare, Qt::red, Qt::red, 8});
    curve2->setLineStyle(QCPCurve::lsNone);
    curve2->setScatterSkip(0);

    // Update on unit changes
    QObject::connect(&unit, &UnitGroup::selectionChanged, this, &ProfileView::updatePlot);
}

void ProfileView::setData(const MatrixXd& data) {
    input = data;
    updatePlot();
}

void ProfileView::setSelection(const QVector<int>& indices) {
    selection = indices;
    updatePlot();
}

void ProfileView::updatePlot() {
    this->xAxis->setLabel("X " + unit.getSelectedUnit().getLabel());
    this->yAxis->setLabel("Y " + unit.getSelectedUnit().getLabel());

    curve0->data()->clear();
    curve1->data()->clear();
    curve2->data()->clear();

    try {
        ProfileCurve profile(input, 0.0, 0.0, 0.0);

        // Add profile curve
        for(double s: Linspace<double>(profile.s_min(), profile.s_max(), 500)) {    // Magic number
            Vector<3> point = profile(s);
            curve0->addData(
                unit.getSelectedUnit().fromBase(point[0]),
                unit.getSelectedUnit().fromBase(point[1])
            );
        }

        // Add control points depending on selection status
        for(int i = 0; i < input.rows(); ++i) {
            Vector<3> point = profile(input(i, 0));
            if(selection.contains(i)) {
                curve2->addData(
                    unit.getSelectedUnit().fromBase(point[0]),
                    unit.getSelectedUnit().fromBase(point[1])
                );
            }
            else {
                curve1->addData(
                    unit.getSelectedUnit().fromBase(point[0]),
                    unit.getSelectedUnit().fromBase(point[1])
                );
            }
        }
    }
    catch(const std::invalid_argument& e) {
        curve0->data()->clear();
        curve1->data()->clear();
        curve2->data()->clear();
    }

    this->rescaleAxes();
    this->replot();
}
