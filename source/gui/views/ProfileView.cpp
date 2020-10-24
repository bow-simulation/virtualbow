#include "ProfileView.hpp"
#include "solver/model//ProfileCurve.hpp"
#include "solver/numerics/Linspace.hpp"

ProfileView::ProfileView() {
    this->xAxis->setLabel("x [m]");
    this->yAxis->setLabel("y [m]");
    this->setAspectPolicy(PlotWidget::SCALE_Y);

    this->addLayer("curve");
    this->addLayer("nodes");
}

void ProfileView::setData(const MatrixXd& data) {
    this->data = data;
    updatePlot();
    this->rescaleAxes();
    this->replot();
}

void ProfileView::setSelection(const QVector<int>& selection) {
    this->selection = selection;
    updateHighlights();
    this->replot();
}

#include <iostream>

void ProfileView::updatePlot() {
    resetPlot();

    try {
        auto input = ProfileCurve::to_input(data);

        std::cout << ">>>>>>>> Profile: <<<<<<<<<\n";
        for(auto& in: input) {
            auto print_optional = [](std::optional<double> option) {
                if(option) {
                    std::cout << *option  << "\n";
                } else {
                    std::cout << "Empty" << "\n";
                }
            };

            std::cout << ">>> Segment:\n";
            std::cout << "length: ";
            print_optional(in.length);

            std::cout << "angle: ";
            print_optional(in.angle);

            std::cout << "delta_x: ";
            print_optional(in.delta_x);

            std::cout << "delta_y: ";
            print_optional(in.delta_y);

            std::cout << std::endl;
        }


        ProfileCurve profile = ProfileCurve(input);

        for(auto& segment: profile.get_segments()) {
            QCPCurve* curve = new QCPCurve(this->xAxis, this->yAxis);
            curve->setLayer("curve");

            const unsigned samples = 150;    // Magic number
            for(double t: Linspace<double>(0.0, 1.0, samples)) {
                curve->addData(segment.x(t), segment.y(t));
            }
            curves.push_back(curve);
        }

        for(auto& point: profile.get_nodes()) {
            QCPCurve* curve = new QCPCurve(this->xAxis, this->yAxis);
            curve->setLayer("nodes");
            curve->setLineStyle(QCPCurve::lsNone);
            curve->setScatterSkip(0);

            curve->addData(point.x, point.y);
            nodes.push_back(curve);
        }
    }
    catch(const std::exception& e) {
        resetPlot();
    }

    updateHighlights();
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
}
