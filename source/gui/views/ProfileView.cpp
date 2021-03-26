#include "ProfileView.hpp"
#include "solver/model//ProfileCurve.hpp"
#include "solver/numerics/Linspace.hpp"

ProfileView::ProfileView(const UnitGroup& unit)
    : unit(unit)
{
    this->setAspectPolicy(PlotWidget::SCALE_Y);
    this->addLayer("curve");
    this->addLayer("nodes");

    label = new QCPItemText(this);
    label->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
    label->position->setType(QCPItemPosition::ptAxisRectRatio);
    label->position->setCoords(0.5, 0.05);
    label->setColor(Qt::red);

    // Update on unit changes
    QObject::connect(&unit, &UnitGroup::selectionChanged, this, &ProfileView::updatePlot);
}

void ProfileView::setData(const MatrixXd& data) {
    this->data = data;
    updatePlot();
}

void ProfileView::setSelection(const QVector<int>& selection) {
    this->selection = selection;
    updateHighlights();
}

#include <iostream>

void ProfileView::updatePlot() {
    resetPlot();

    this->xAxis->setLabel("X " + unit.getSelectedUnit().getLabel());
    this->yAxis->setLabel("Y " + unit.getSelectedUnit().getLabel());

    auto input = ProfileCurve::input_from_matrix(data);
    std::optional<std::string> error = ProfileCurve::validate_input(input);
    if(error) {
        label->setText(QString::fromStdString(*error));
        return;
    }

    try {
        ProfileCurve profile = ProfileCurve(input);
        for(auto& segment: profile.get_segments()) {
            QCPCurve* curve = new QCPCurve(this->xAxis, this->yAxis);
            curve->setLayer("curve");

            const unsigned samples = 150;    // Magic number
            for(double t: Linspace<double>(0.0, 1.0, samples)) {
                curve->addData(
                    unit.getSelectedUnit().fromBase(segment.x(t)),
                    unit.getSelectedUnit().fromBase(segment.y(t))
                );
            }
            curves.push_back(curve);
        }

        for(auto& point: profile.get_nodes()) {
            QCPCurve* curve = new QCPCurve(this->xAxis, this->yAxis);
            curve->setLayer("nodes");
            curve->setLineStyle(QCPCurve::lsNone);
            curve->setScatterSkip(0);

            curve->addData(
                unit.getSelectedUnit().fromBase(point.x),
                unit.getSelectedUnit().fromBase(point.y)
            );
            nodes.push_back(curve);
        }

        label->setText("");
        updateHighlights();
    }
    catch(const std::exception& e) {
        label->setText(e.what());
    }

    this->rescaleAxes();
    this->replot();
}

void ProfileView::resetPlot() {
    for(auto curve: curves) {
        this->removePlottable(curve);
    }
    curves.clear();

    for(auto node: nodes) {
        this->removePlottable(node);
    }
    nodes.clear();
}

void ProfileView::updateHighlights() {
    for(int i = 0; i < curves.size(); ++i) {
        if(selection.contains(i)) {
            curves[i]->setPen({Qt::red, 2});
        }
        else {
            curves[i]->setPen({Qt::blue, 2});
        }
    }

    for(int i = 0; i < nodes.size(); ++i) {
        if((i < curves.size() && selection.contains(i)) || (i > 0 && selection.contains(i-1))) {
            nodes[i]->setScatterStyle({QCPScatterStyle::ssSquare, Qt::red, Qt::red, 8});
        }
        else {
            nodes[i]->setScatterStyle({QCPScatterStyle::ssSquare, Qt::blue, 8});
        }
    }

    this->replot();
}
