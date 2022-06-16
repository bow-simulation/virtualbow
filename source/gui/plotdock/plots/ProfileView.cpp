#include "ProfileView.hpp"
#include "solver/model/profile/ProfileCurve.hpp"
#include "solver/numerics/Linspace.hpp"

// Magic numbers
const size_t N_SEGMENT_POINTS   = 100;
const size_t N_CURVATURE_POINTS = 200;
const double CURVATURE_SCALING  = 0.05;

ProfileView::ProfileView(const UnitGroup& unit)
    : unit(unit)
{
    this->setAspectPolicy(PlotWidget::SCALE_Y);

    // Menu actions

    action_show_curvature = new QAction("Show curvature", this);
    action_show_curvature->setCheckable(true);
    action_show_curvature->setChecked(false);
    QObject::connect(action_show_curvature, &QAction::triggered, [&] {
        updateVisibility();
        this->replot();
    });

    action_show_nodes = new QAction("Show nodes", this);
    action_show_nodes->setCheckable(true);
    action_show_nodes->setChecked(true);
    QObject::connect(action_show_nodes, &QAction::triggered, [&] {
        updateVisibility();
        this->replot();
    });

    QAction* before = this->contextMenu()->actions().isEmpty() ? nullptr : this->contextMenu()->actions().front();
    this->contextMenu()->insertAction(before, action_show_curvature);
    this->contextMenu()->insertAction(before, action_show_nodes);
    this->contextMenu()->insertSeparator(before);

    // Update on unit changes
    QObject::connect(&unit, &UnitGroup::selectionChanged, this, &ProfileView::updatePlot);
}

void ProfileView::setData(const std::vector<SegmentInput>& data) {
    input = data;
    updatePlot();
}

void ProfileView::setSelection(const QList<int>& indices) {
    selection = indices;
    updateSelection();
    this->replot();
}

void ProfileView::updatePlot() {
    this->xAxis->setLabel("X " + unit.getSelectedUnit().getLabel());
    this->yAxis->setLabel("Y " + unit.getSelectedUnit().getLabel());

    this->clearPlottables();

    // Construct profile curve segment by segment, stop on first error
    ProfileCurve profile;
    try {
        for(auto& segment: input) {
            profile.add_segment(segment);
        }
    }
    catch(const std::exception& e) {
        // TODO: Show error on plot
    }

    // Abort the curve is empty, otherwise draw the correct segments
    if(profile.get_segments().empty()) {
        return;
    }

    // Add curvature
    std::vector<double> s = linspace(0.0, profile.length(), N_CURVATURE_POINTS);
    std::vector<double> k; k.reserve(s.size());
    for(double si: s) {
        k.push_back(profile.curvature(si));
    }

    double k_max = *std::max_element(k.begin(), k.end(), [](double a, double b){ return std::abs(a) < std::abs(b); });
    double scale = (k_max != 0.0) ? CURVATURE_SCALING*profile.length()/std::abs(k_max) : 0.0;

    auto curvature_outline = new QCPCurve(this->xAxis, this->yAxis);
    curvature_outline->setName("Curvature outline");
    curvature_outline->setPen({Qt::darkGray, 1});
    curvature_outline->setScatterSkip(0);

    curvature_lines.clear();
    curvature_lines.push_back(curvature_outline);

    for(size_t i = 0; i < s.size(); ++i) {
        Vector<2> position = profile.position(s[i]);
        double angle = profile.angle(s[i]);

        double x_start = unit.getSelectedUnit().fromBase(position(0));
        double y_start = unit.getSelectedUnit().fromBase(position(1));
        double x_end = unit.getSelectedUnit().fromBase(position(0) - scale*k[i]*sin(angle));
        double y_end = unit.getSelectedUnit().fromBase(position(1) + scale*k[i]*cos(angle));

        auto line = new QCPCurve(this->xAxis, this->yAxis);
        line->setPen({Qt::lightGray, 1});
        line->setScatterSkip(0);
        line->addData(x_start, y_start);
        line->addData(x_end, y_end);

        curvature_outline->addData(x_end, y_end);
        curvature_lines.push_back(line);
    }

    // Add profile segments
    segment_curves.clear();
    for(size_t i = 0; i < profile.get_nodes().size()-1; ++i) {
        auto segment_curve = new QCPCurve(this->xAxis, this->yAxis);
        segment_curve->setName("Segment");
        segment_curves.push_back(segment_curve);

        for(double s: Linspace<double>(profile.get_nodes()[i].s, profile.get_nodes()[i+1].s, N_SEGMENT_POINTS)) {
            Vector<2> point = profile.position(s);
            segment_curve->addData(
                unit.getSelectedUnit().fromBase(point(0)),
                unit.getSelectedUnit().fromBase(point(1))
            );
        }
    }

    // Add profile nodes
    segment_nodes.clear();

    std::vector<Point> nodes;
    for(auto& segment: profile.get_segments()) {
        std::vector<Point> segment_nodes = segment->nodes();
        nodes.insert(nodes.end(), segment_nodes.begin(), segment_nodes.end());
    }

    for(auto& node: nodes) {
        auto segment_node = new QCPCurve(this->xAxis, this->yAxis);
        segment_node->setName("Node");
        segment_nodes.push_back(segment_node);
        segment_node->addData(
            unit.getSelectedUnit().fromBase(node.position(0)),
            unit.getSelectedUnit().fromBase(node.position(1))
        );
    }

    updateSelection();
    updateVisibility();

    this->rescaleAxes();
    this->replot();
}

void ProfileView::updateSelection() {
    for(int i = 0; i < segment_curves.size(); ++i) {
        if(selection.contains(i)) {
            segment_curves[i]->setPen({Qt::red, 2});
            segment_curves[i]->setScatterSkip(0);
        }
        else {
            segment_curves[i]->setPen({Qt::blue, 2});
            segment_curves[i]->setScatterSkip(0);
        }
    }

    for(int i = 0; i < segment_nodes.size(); ++i) {
        if(selection.contains(i) || selection.contains(i-1)) {
            segment_nodes[i]->setScatterStyle({QCPScatterStyle::ssSquare, Qt::red, 8});
            segment_nodes[i]->setLineStyle(QCPCurve::lsNone);
            segment_nodes[i]->setScatterSkip(0);
        }
        else {
            segment_nodes[i]->setScatterStyle({QCPScatterStyle::ssSquare, Qt::blue, 8});
            segment_nodes[i]->setLineStyle(QCPCurve::lsNone);
            segment_nodes[i]->setScatterSkip(0);
        }
    }
}

void ProfileView::updateVisibility() {
    for(auto line: curvature_lines) {
        line->setVisible(action_show_curvature->isChecked());
    }

    for(auto segment_node: segment_nodes) {
        segment_node->setVisible(action_show_nodes->isChecked());
    }
}
