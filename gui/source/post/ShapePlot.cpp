#include "ShapePlot.hpp"
#include "pre/viewmodel/units/UnitSystem.hpp"

ShapePlot::ShapePlot(const Common& common, const States& states, int background_states)
    : common(common),
      states(states),
      quantity(Quantities::length),
      background_states(background_states),
      index(0)
{
    this->setAspectPolicy(PlotWidget::SCALE_Y);

    // Curves for background states (index: 0 ... background_states - 1)

    for(int i = 0; i < background_states; ++i) {
        limb_right.append(new QCPCurve(this->xAxis, this->yAxis));
        limb_right.back()->setPen({Qt::lightGray, 1.0});
        limb_right.back()->setScatterSkip(0);

        limb_left.append(new QCPCurve(this->xAxis, this->yAxis));
        limb_left.back()->setPen({Qt::lightGray, 1.0});
        limb_left.back()->setScatterSkip(0);

        string_right.append(new QCPCurve(this->xAxis, this->yAxis));
        string_right.back()->setPen({Qt::lightGray, 1.0});
        string_right.back()->setScatterSkip(0);

        string_left.append(new QCPCurve(this->xAxis, this->yAxis));
        string_left.back()->setPen({Qt::lightGray, 1.0});
        string_left.back()->setScatterSkip(0);
    }

    // Curves for current state (index: background_state)

    limb_right.append(new QCPCurve(this->xAxis, this->yAxis));
    limb_right.back()->setName("Limb right");
    limb_right.back()->setPen({Qt::blue, 2.0});
    limb_right.back()->setScatterSkip(0);

    limb_left.append(new QCPCurve(this->xAxis, this->yAxis));
    limb_left.back()->setName("Limb left");
    limb_left.back()->setPen({Qt::blue, 2.0});
    limb_left.back()->setScatterSkip(0);

    string_right.append(new QCPCurve(this->xAxis, this->yAxis));
    string_right.back()->setName("String right");
    string_right.back()->setPen({Qt::blue, 2.0});
    string_right.back()->setScatterSkip(0);

    string_left.append(new QCPCurve(this->xAxis, this->yAxis));
    string_left.back()->setName("String left");
    string_left.back()->setPen({Qt::blue, 2.0});
    string_left.back()->setScatterSkip(0);

    arrow = new QCPCurve(this->xAxis, this->yAxis);
    arrow->setName("Arrow");
    arrow->setLineStyle(QCPCurve::lsNone);
    arrow->setScatterStyle({QCPScatterStyle::ssCrossCircle, Qt::red, 10});

    QObject::connect(&quantity, &Quantity::unitChanged, this, &ShapePlot::updatePlot);
    updatePlot();
}

void ShapePlot::setStateIndex(int i) {
    index = i;
    updateCurrentState();
    this->replot();
}

void ShapePlot::updatePlot() {
    updateBackgroundStates();
    updateCurrentState();
    updateAxes();

    this->replot();
}

void ShapePlot::updateBackgroundStates() {

    int intermediate_states = background_states - 1;    // Number of states from brace to full draw, excluding the unbraced state

    for(int i = 0; i < intermediate_states; ++i) {
        size_t j = (intermediate_states == 1) ? 0 : i*(states.time.size() - 1)/(intermediate_states - 1);
        plotLimbOutline(limb_left[i], limb_right[i], states.limb_pos[j]);
        plotString(string_left[i], string_right[i], states.string_pos[j]);
    }

    if(intermediate_states >= 0) {
        // Unbraced state
        plotLimbOutline(limb_left[intermediate_states], limb_right[intermediate_states], common.limb.position);
    }
}

void ShapePlot::updateCurrentState() {
    plotLimbOutline(limb_left.back(), limb_right.back(), states.limb_pos[index]);
    plotString(string_left.back(), string_right.back(), states.string_pos[index]);
    plotArrow(states.arrow_pos[index]);
}

void ShapePlot::updateAxes() {
    this->xAxis->setLabel("X " + quantity.getUnit().getLabel());
    this->yAxis->setLabel("Y " + quantity.getUnit().getLabel());

    QCPRange x_range;
    QCPRange y_range;

    auto expand2 = [&](const std::vector<std::array<double, 2>>& position) {
        for(size_t i = 0; i < position.size(); ++i) {
            x_range.expand(quantity.getUnit().fromBase( position[i][0]));
            x_range.expand(quantity.getUnit().fromBase(-position[i][0]));
            y_range.expand(quantity.getUnit().fromBase( position[i][1]));
        }
    };

    auto expand3 = [&](const std::vector<std::array<double, 3>>& position) {
        for(size_t i = 0; i < position.size(); ++i) {
            x_range.expand(quantity.getUnit().fromBase( position[i][0]));
            x_range.expand(quantity.getUnit().fromBase(-position[i][0]));
            y_range.expand(quantity.getUnit().fromBase( position[i][1]));
        }
    };

    expand3(common.limb.position);
    for(size_t i = 0; i < states.time.size(); ++i) {
        // Add 0.5*height as an estimated upper bound
        //expand(states.x_pos_limb[i] + 0.5*limb.height, states.y_pos_limb[i] + 0.5*limb.height);
        //expand(states.x_pos_string[i], states.y_pos_string[i]);
        expand3(states.limb_pos[i]);
        expand2(states.string_pos[i]);
    }

    this->setAxesLimits(x_range, y_range);
}

void ShapePlot::plotLimbOutline(QCPCurve* left, QCPCurve* right, const std::vector<std::array<double, 3>>& position) {
    left->data()->clear();
    right->data()->clear();

    // Iterate forward and draw back
    for(int i = 0; i < position.size(); ++i) {
        double xi = position[i][0];
        double yi = position[i][1];

        left->addData(
            quantity.getUnit().fromBase(-xi),
            quantity.getUnit().fromBase( yi)
        );
        right->addData(
            quantity.getUnit().fromBase(xi),
            quantity.getUnit().fromBase(yi)
        );
    }

    // Iterate backward and plot belly
    for(int i = position.size() - 1; i >= 0; --i) {
        double xi = position[i][0] + common.limb.height[i]*sin(position[i][2]);
        double yi = position[i][1] - common.limb.height[i]*cos(position[i][2]);

        left->addData(
            quantity.getUnit().fromBase(-xi),
            quantity.getUnit().fromBase( yi)
        );
        right->addData(
            quantity.getUnit().fromBase(xi),
            quantity.getUnit().fromBase(yi)
        );
    }
}

void ShapePlot::plotString(QCPCurve* left, QCPCurve* right, const std::vector<std::array<double, 2>>& position) {
    left->data()->clear();
    right->data()->clear();

    for(int i = 0; i < position.size(); ++i) {
        double xi = position[i][0];
        double yi = position[i][1];

        left->addData(
            quantity.getUnit().fromBase(-xi),
            quantity.getUnit().fromBase( yi)
        );
        right->addData(
            quantity.getUnit().fromBase(xi),
            quantity.getUnit().fromBase(yi)
        );
    }
}

void ShapePlot::plotArrow(double position) {
    arrow->data()->clear();
    arrow->addData(
        quantity.getUnit().fromBase(0.0),
        quantity.getUnit().fromBase(position)
    );
}
