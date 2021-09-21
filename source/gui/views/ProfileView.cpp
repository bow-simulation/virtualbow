#include "ProfileView.hpp"
#include "solver/model/ProfileCurve.hpp"
#include "solver/numerics/Linspace.hpp"

// Magic numbers
const size_t N_PROFILE_POINTS   = 500;
const size_t N_CURVATURE_POINTS = 200;
const double CURVATURE_SCALING  = 0.05;

ProfileView::ProfileView(const UnitGroup& unit)
    : unit(unit)
{
    this->setAspectPolicy(PlotWidget::SCALE_Y);

    // Curvature lines
    curvature_outline = new QCPCurve(this->xAxis, this->yAxis);
    curvature_outline->setPen({Qt::blue, 1});
    curvature_outline->setScatterSkip(0);
    for(size_t i = 0; i < N_CURVATURE_POINTS; ++i) {
        auto line = new QCPCurve(this->xAxis, this->yAxis);
        line->setPen({Qt::darkGray, 1});
        line->setScatterSkip(0);
        curvature_lines.append(line);
    }

    // Profile curve
    profile_curve = new QCPCurve(this->xAxis, this->yAxis);
    profile_curve->setPen({Qt::blue, 2});
    profile_curve->setScatterSkip(0);

    // Unselected nodes
    profile_nodes_unselected = new QCPCurve(this->xAxis, this->yAxis);
    profile_nodes_unselected->setScatterStyle({QCPScatterStyle::ssSquare, Qt::blue, 8});
    profile_nodes_unselected->setLineStyle(QCPCurve::lsNone);
    profile_nodes_unselected->setScatterSkip(0);

    // Selected nodes
    profile_nodes_selected = new QCPCurve(this->xAxis, this->yAxis);
    profile_nodes_selected->setScatterStyle({QCPScatterStyle::ssSquare, Qt::red, Qt::red, 8});
    profile_nodes_selected->setLineStyle(QCPCurve::lsNone);
    profile_nodes_selected->setScatterSkip(0);

    // Menu actions

    auto action_show_curvature = new QAction("Show curvature", this);
    action_show_curvature->setCheckable(true);
    action_show_curvature->setChecked(false);
    QObject::connect(action_show_curvature, &QAction::triggered, [&](bool checked) {
        setCurvatureVisible(checked);
        this->replot();
    });

    auto action_show_nodes = new QAction("Show nodes", this);
    action_show_nodes->setCheckable(true);
    action_show_nodes->setChecked(true);
    QObject::connect(action_show_nodes, &QAction::triggered, [&](bool checked) {
        setNodesVisible(checked);
        this->replot();
    });

    QAction* before = this->contextMenu()->actions().isEmpty() ? nullptr : this->contextMenu()->actions().front();
    this->contextMenu()->insertAction(before, action_show_curvature);
    this->contextMenu()->insertAction(before, action_show_nodes);
    this->contextMenu()->insertSeparator(before);

    setCurvatureVisible(action_show_curvature->isChecked());
    setNodesVisible(action_show_nodes->isChecked());

    // Update on unit changes
    QObject::connect(&unit, &UnitGroup::selectionChanged, this, &ProfileView::updatePlot);
}

void ProfileView::setData(const std::vector<SegmentInput>& data) {
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

    try {
        ProfileCurve profile(input, 0.0, 0.0, 0.0);

        // Add profile curve

        profile_curve->data()->clear();
        for(double s: Linspace<double>(profile.s_min(), profile.s_max(), N_PROFILE_POINTS)) {
            CurvePoint point = profile(s);
            profile_curve->addData(
                unit.getSelectedUnit().fromBase(point.x),
                unit.getSelectedUnit().fromBase(point.y)
            );
        }

        // Add control points depending on selection status

        /*
        profile_nodes_unselected->data()->clear();
        profile_nodes_selected->data()->clear();
        for(int i = 0; i < input.rows(); ++i) {
            CurvePoint point = profile(input(i, 0));
            if(selection.contains(i)) {
                profile_nodes_selected->addData(
                    unit.getSelectedUnit().fromBase(point.x),
                    unit.getSelectedUnit().fromBase(point.y)
                );
            }
            else {
                profile_nodes_unselected->addData(
                    unit.getSelectedUnit().fromBase(point.x),
                    unit.getSelectedUnit().fromBase(point.y)
                );
            }
        }
        */

        // Add curvature

        /*
        double k_max = input.col(1).cwiseAbs().maxCoeff();
        const double scale = CURVATURE_SCALING*(profile.s_max() - profile.s_min())/k_max;

        curvature_outline->data()->clear();
        std::vector<double> s = linspace(profile.s_min(), profile.s_max(), curvature_lines.size());
        for(size_t i = 0; i < s.size(); ++i) {
            CurvePoint point = profile(s[i]);
            double x_start = unit.getSelectedUnit().fromBase(point.x);
            double y_start = unit.getSelectedUnit().fromBase(point.y);
            double x_end = unit.getSelectedUnit().fromBase(point.x - scale*point.k*sin(point.phi));
            double y_end = unit.getSelectedUnit().fromBase(point.y + scale*point.k*cos(point.phi));

            curvature_outline->addData(x_end, y_end);
            curvature_lines[i]->data()->clear();
            curvature_lines[i]->addData(x_start, y_start);
            curvature_lines[i]->addData(x_end, y_end);
        }
        */
    }
    catch(const std::invalid_argument& e) {
        // TODO: Show error message on plot
    }

    this->rescaleAxes();
    this->replot();
}

void ProfileView::setCurvatureVisible(bool visible) {
    curvature_outline->setVisible(visible);
    for(auto line: curvature_lines) {
        line->setVisible(visible);
    }
}

void ProfileView::setNodesVisible(bool visible) {
    profile_nodes_unselected->setVisible(visible);
    profile_nodes_selected->setVisible(visible);
}
