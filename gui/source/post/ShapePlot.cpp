#include "ShapePlot.hpp"

ShapePlot::ShapePlot(const LimbProperties& limb, const BowStates& states, int background_states)
    : limb(limb),
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

        plotLimbOutline(limb_left[i], limb_right[i], states.x_pos_limb[j], states.y_pos_limb[j], states.angle_limb[j]);
        string_left[i]->setData(
            quantity.getUnit().fromBase(-states.x_pos_string[j]),
            quantity.getUnit().fromBase(states.y_pos_string[j])
        );
        string_right[i]->setData(
            quantity.getUnit().fromBase(states.x_pos_string[j]),
            quantity.getUnit().fromBase(states.y_pos_string[j])
        );
    }

    if(intermediate_states >= 0) {
        // Unbraced state
        plotLimbOutline(limb_left[intermediate_states], limb_right[intermediate_states], limb.x_pos, limb.y_pos, limb.angle);
    }
}

void ShapePlot::updateCurrentState() {
    plotLimbOutline(limb_left.back(), limb_right.back(), states.x_pos_limb[index], states.y_pos_limb[index], states.angle_limb[index]);

    string_right.back()->setData(
        quantity.getUnit().fromBase(states.x_pos_string[index]),
        quantity.getUnit().fromBase(states.y_pos_string[index])
    );
    string_left.back()->setData(
        quantity.getUnit().fromBase(-states.x_pos_string[index]),
        quantity.getUnit().fromBase(states.y_pos_string[index])
    );

    arrow->data()->clear();
    arrow->addData(
        quantity.getUnit().fromBase(0.0),
        quantity.getUnit().fromBase(states.pos_arrow[index])
    );
}

void ShapePlot::updateAxes() {
    this->xAxis->setLabel("X " + quantity.getUnit().getLabel());
    this->yAxis->setLabel("Y " + quantity.getUnit().getLabel());

    QCPRange x_range;
    QCPRange y_range;

    auto expand = [&](const VectorXd& x_values, const VectorXd& y_values) {
        for(size_t i = 0; i < x_values.size(); ++i) {
            x_range.expand(quantity.getUnit().fromBase( x_values[i]));
            x_range.expand(quantity.getUnit().fromBase(-x_values[i]));
            y_range.expand(quantity.getUnit().fromBase( y_values[i]));
        }
    };

    expand(limb.x_pos, limb.y_pos);
    for(size_t i = 0; i < states.time.size(); ++i) {
        // Add 0.5*height as an estimated upper bound
        expand(states.x_pos_limb[i] + 0.5*limb.height, states.y_pos_limb[i] + 0.5*limb.height);
        expand(states.x_pos_string[i], states.y_pos_string[i]);
    }

    this->setAxesLimits(x_range, y_range);
}

void ShapePlot::plotLimbOutline(QCPCurve* left, QCPCurve* right, const VectorXd& x, const VectorXd& y, const VectorXd& phi) {
    left->data()->clear();
    right->data()->clear();

    // Iterate forward and draw back
    for(int i = 0; i < phi.size(); ++i) {
        double xi = x[i];
        double yi = y[i];
        left->addData(
            quantity.getUnit().fromBase(-xi),
            quantity.getUnit().fromBase(yi)
        );
        right->addData(
            quantity.getUnit().fromBase(xi),
            quantity.getUnit().fromBase(yi)
        );
    }

    // Iterate backward and plot belly
    for(int i = phi.size() - 1; i >= 0; --i) {
        double xi = x[i] + limb.height[i]*sin(phi[i]);
        double yi = y[i] - limb.height[i]*cos(phi[i]);
        left->addData(
            quantity.getUnit().fromBase(-xi),
            quantity.getUnit().fromBase(yi)
        );
        right->addData(
            quantity.getUnit().fromBase(xi),
            quantity.getUnit().fromBase(yi)
        );
    }
}
