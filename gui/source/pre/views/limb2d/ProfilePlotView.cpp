#include "ProfilePlotView.hpp"
#include "pre/models/MainModel.hpp"
#include "pre/models/units/UnitSystem.hpp"

// Magic number, determines offset of the curvature outline (fraction of curve length)
const double CURVATURE_SCALING  = 0.05;

ProfilePlotView::ProfilePlotView(MainModel* model):
    model(model)
{
    this->setAspectPolicy(PlotWidget::SCALE_Y);

    // Line
    curveLine = new QCPCurve(this->xAxis, this->yAxis);
    curveLine->setName("Line");
    curveLine->setPen({Qt::blue, 2});

    // Control points
    curveNodes = new QCPCurve(this->xAxis, this->yAxis);
    curveNodes->setName("Points");
    curveNodes->setScatterStyle({QCPScatterStyle::ssSquare, Qt::blue, 8});
    curveNodes->setLineStyle(QCPCurve::lsNone);

    // Curvature visualization
    curvatureOutline = new QCPCurve(this->xAxis, this->yAxis);
    curvatureOutline->setName("Curvature outline");
    curvatureOutline->setPen(Qt::NoPen);
    curvatureOutline->setBrush(QBrush(QColor(0, 0, 255, 35)));
    curvatureOutline->setScatterSkip(0);

    // Menu actions

    action_show_curvature = new QAction("Show curvature", this);
    action_show_curvature->setCheckable(true);
    action_show_curvature->setChecked(false);
    QObject::connect(action_show_curvature, &QAction::triggered, [&] {
        updateVisibility();
        replot();
    });

    action_show_nodes = new QAction("Show nodes", this);
    action_show_nodes->setCheckable(true);
    action_show_nodes->setChecked(true);
    QObject::connect(action_show_nodes, &QAction::triggered, [&] {
        updateVisibility();
        replot();
    });

    QAction* before = contextMenu()->actions().isEmpty() ? nullptr : contextMenu()->actions().front();
    contextMenu()->insertAction(before, action_show_curvature);
    contextMenu()->insertAction(before, action_show_nodes);
    contextMenu()->insertSeparator(before);

    // Update on unit and geometry changes
    QObject::connect(&Quantities::length, &Quantity::unitChanged, this, &ProfilePlotView::updatePlot);
    QObject::connect(model, &MainModel::geometryChanged, this, &ProfilePlotView::updatePlot);

    // Initial update
    updatePlot();
}

void ProfilePlotView::updatePlot() {
    this->xAxis->setLabel("X " + Quantities::length.getUnit().getLabel());
    this->yAxis->setLabel("Y " + Quantities::length.getUnit().getLabel());

    curveLine->data()->clear();
    curveNodes->data()->clear();
    curvatureOutline->data()->clear();


    if(model->hasGeometry()) {
        auto& position = model->getGeometry().position_eval;
        auto& curvature = model->getGeometry().curvature_eval;

        // Determine scaling of the curvature outline from curve length and maximum curvature
        double k_max = *std::max_element(curvature.begin(), curvature.end(), [](double a, double b){ return std::abs(a) < std::abs(b); });
        double scale = (k_max != 0.0) ? CURVATURE_SCALING*model->getGeometry().length.back()/std::abs(k_max) : 0.0;

        // Plot outline of the curvature curve
        for(size_t i = 0; i < position.size(); ++i) {
            curvatureOutline->addData(
                Quantities::length.getUnit().fromBase(position[i][0] - scale*curvature[i]*sin(position[i][2])),
                Quantities::length.getUnit().fromBase(position[i][1] + scale*curvature[i]*cos(position[i][2]))
            );
        }

        // Close the loop by adding the profile curve points
        for(int i = static_cast<int>(position.size()) - 1; i >= 0; --i) {
            curvatureOutline->addData(
                Quantities::length.getUnit().fromBase(position[i][0]),
                Quantities::length.getUnit().fromBase(position[i][1])
            );
        }

        // Plot profile curve
        for(auto& point: model->getGeometry().position_eval) {
            curveLine->addData(
                Quantities::length.getUnit().fromBase(point[0]),
                Quantities::length.getUnit().fromBase(point[1])
            );
        }

        // Plot profile nodes
        for(auto& point: model->getGeometry().position_control) {
            curveNodes->addData(
                Quantities::length.getUnit().fromBase(point[0]),
                Quantities::length.getUnit().fromBase(point[1])
            );
        }
    }

    updateVisibility();
    rescaleAxes();
    replot();
}

void ProfilePlotView::updateSelection() {
    /*
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
    */
}

void ProfilePlotView::updateVisibility() {
    curveNodes->setVisible(action_show_nodes->isChecked());
    curvatureOutline->setVisible(action_show_curvature->isChecked());
}
