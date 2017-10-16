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

void ShapePlot::setStateIndex(int index)
{
    limb_right->setData(states.x_limb[index], states.y_limb[index]);
    limb_left->setData(-states.x_limb[index], states.y_limb[index]);

    string_right->setData(states.x_string[index], states.y_string[index]);
    string_left->setData(-states.x_string[index], states.y_string[index]);

    arrow->setData(std::array<double, 1>{0.0}, std::array<double, 1>{states.pos_arrow[index]});

    this->replot();
}

void ShapePlot::plotIntermediateStates()
{
    // Unbraced state
    auto limb_right = new QCPCurve(this->xAxis, this->yAxis);
    limb_right->setData(setup.limb.x, setup.limb.y);
    limb_right->setPen({Qt::lightGray, 2});
    limb_right->setScatterSkip(0);    // Todo: Having to explicitly state this is retarded

    auto limb_left = new QCPCurve(this->xAxis, this->yAxis);
    limb_left->setData(-setup.limb.x, setup.limb.y);
    limb_left->setPen({Qt::lightGray, 2});
    limb_left->setScatterSkip(0);    // Todo: Having to explicitly state this is retarded


    // Stroboscope-like plots during intermediate states
    unsigned steps = 2; // Todo: Magic number
    for(unsigned i = 0; i <= steps; ++i)
    {
        size_t j = i*(states.time.size()-1)/steps;

        auto limb_right = new QCPCurve(this->xAxis, this->yAxis);
        limb_right->setData(states.x_limb[j], states.y_limb[j]);
        limb_right->setPen({Qt::lightGray, 2});
        limb_right->setScatterSkip(0);    // Todo: Having to explicitly state this is retarded

        auto limb_left = new QCPCurve(this->xAxis, this->yAxis);
        limb_left->setData(-states.x_limb[j], states.y_limb[j]);
        limb_left->setPen({Qt::lightGray, 2});
        limb_left->setScatterSkip(0);    // Todo: Having to explicitly state this is retarded

        auto string_right = new QCPCurve(this->xAxis, this->yAxis);
        string_right->setData(states.x_string[j], states.y_string[j]);
        string_right->setPen({Qt::lightGray, 1});
        string_right->setScatterSkip(0);    // Todo: Having to explicitly state this is retarded

        auto string_left = new QCPCurve(this->xAxis, this->yAxis);
        string_left->setData(-states.x_string[j], states.y_string[j]);
        string_left->setPen({Qt::lightGray, 1});
        string_left->setScatterSkip(0);    // Todo: Having to explicitly state this is retarded
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
