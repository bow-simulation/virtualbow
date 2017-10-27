#include "ShapePlot.hpp"

ShapePlot::ShapePlot(const SetupData& setup, const BowStates& states, bool intermediate_states)
    : setup(setup),
      states(states)
{
    if(intermediate_states)
        plotIntermediateStates();

    limb_right = new QCPCurve(this->xAxis, this->yAxis);
    limb_right->setPen({Qt::blue, 2});
    limb_right->setScatterSkip(0);    // Todo: Having to explicitly state this is retarded

    limb_left = new QCPCurve(this->xAxis, this->yAxis);
    limb_left->setPen({Qt::blue, 2});
    limb_left->setScatterSkip(0);    // Todo: Having to explicitly state this is retarded

    string_right = new QCPCurve(this->xAxis, this->yAxis);
    string_right->setPen({Qt::blue, 1});
    string_right->setScatterSkip(0);    // Todo: Having to explicitly state this is retarded

    string_left = new QCPCurve(this->xAxis, this->yAxis);
    string_left->setPen({Qt::blue, 1});
    string_left->setScatterSkip(0);    // Todo: Having to explicitly state this is retarded

    arrow = new QCPCurve(this->xAxis, this->yAxis);
    arrow->setLineStyle(QCPCurve::lsNone);
    arrow->setScatterStyle({QCPScatterStyle::ssCrossCircle, Qt::red, 10});

    this->xAxis->setLabel("X [m]");
    this->yAxis->setLabel("Y [m]");
    this->setAspectPolicy(PlotWidget::SCALE_Y);
    setAxesRanges();
}

void ShapePlot::setStateIndex(int i)
{
    plotLimbOutline(limb_left, limb_right, states.x_limb[i], states.y_limb[i], states.phi_limb[i]);

    string_right->setData(states.x_string[i], states.y_string[i]);
    string_left->setData(-states.x_string[i], states.y_string[i]);

    arrow->data()->clear();
    arrow->addData(0.0, states.pos_arrow[i]);

    this->replot();
}

void ShapePlot::plotIntermediateStates()
{
    // Unbraced state
    auto limb_left = new QCPCurve(this->xAxis, this->yAxis);
    auto limb_right = new QCPCurve(this->xAxis, this->yAxis);
    plotLimbOutline(limb_left, limb_right, setup.limb.x, setup.limb.y, setup.limb.phi);

    limb_left->setPen({Qt::lightGray, 2});
    limb_left->setScatterSkip(0);

    limb_right->setPen({Qt::lightGray, 2});
    limb_right->setScatterSkip(0);

    // Stroboscope-like plots during intermediate states
    const unsigned steps = 2; // Todo: Magic number
    for(unsigned i = 0; i <= steps; ++i)
    {
        size_t j = i*(states.time.size()-1)/steps;

        auto limb_left = new QCPCurve(this->xAxis, this->yAxis);
        auto limb_right = new QCPCurve(this->xAxis, this->yAxis);
        plotLimbOutline(limb_left, limb_right, states.x_limb[j], states.y_limb[j], states.phi_limb[j]);

        limb_right->setPen({Qt::lightGray, 2});
        limb_right->setScatterSkip(0);

        limb_left->setPen({Qt::lightGray, 2});
        limb_left->setScatterSkip(0);

        auto string_right = new QCPCurve(this->xAxis, this->yAxis);
        string_right->setData(states.x_string[j], states.y_string[j]);
        string_right->setPen({Qt::lightGray, 1});
        string_right->setScatterSkip(0);

        auto string_left = new QCPCurve(this->xAxis, this->yAxis);
        string_left->setData(-states.x_string[j], states.y_string[j]);
        string_left->setPen({Qt::lightGray, 1});
        string_left->setScatterSkip(0);
    }
}

void ShapePlot::plotLimbOutline(QCPCurve* left, QCPCurve* right, const VectorXd& x, const VectorXd& y, const VectorXd& phi)
{
    left->setData(-x, y);
    right->setData(x, y);

    for(int i = phi.size()-1; i >= 0; --i)
    {
        double xi = x[i] + setup.limb.h[i]*sin(phi[i]);
        double yi = y[i] - setup.limb.h[i]*cos(phi[i]);
        left->addData(-xi, yi);
        right->addData(xi, yi);
    }
}

void ShapePlot::setAxesRanges()
{
    QCPRange x_range;
    QCPRange y_range;
    auto expand = [&x_range, &y_range](const VectorXd& x_values, const VectorXd& y_values)
    {
        for(size_t i = 0; i < x_values.size(); ++i)
        {
            x_range.expand( x_values[i]);
            x_range.expand(-x_values[i]);
            y_range.expand( y_values[i]);
        }
    };

    expand(setup.limb.x, setup.limb.y);
    for(size_t i = 0; i < states.time.size(); ++i)
    {
        expand(states.x_limb[i], states.y_limb[i]);
        expand(states.x_string[i], states.y_string[i]);
    }

    this->setAxesLimits(x_range, y_range);
}
