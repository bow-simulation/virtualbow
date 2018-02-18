#include "InputData.hpp"
#include "Compatibility.hpp"
#include <fstream>

void InputData::load(const std::string& path)
{
    std::ifstream stream(path);
    json obj; obj << stream;

    convert_to_current(obj);
    from_json(obj, *this);
}

void InputData::save(const std::string& path) const
{
    json obj;
    to_json(obj, *this);

    std::ofstream stream(path);
    stream << std::setw(4) << obj << std::endl;
}

/*
create_constraint(material.rho, "Density must be positive",         [](double x){ return x > 0; });
create_constraint(material.E,   "Elastic modulus must be positive", [](double x){ return x > 0; });

create_constraint(string.strand_stiffness, "Strand stiffness must be positive",  [](double x){ return x > 0; });
create_constraint(string.strand_density,   "Strand density must be positive",    [](double x){ return x > 0; });
create_constraint(string.n_strands,        "Number of strands must be positive", [](double x){ return x > 0; });

create_constraint(masses.string_center, "String center mass must be positive", [](double x){ return x > 0; });
create_constraint(masses.string_tip,    "String tip mass must be positive",    [](double x){ return x > 0; });
create_constraint(masses.limb_tip,      "Limb tip mass must be positive",      [](double x){ return x > 0; });

create_constraint(operation.draw_length, operation.brace_height, "Draw length must be larger than brace height", [](double x, double y){ return x > y; });
create_constraint(operation.arrow_mass, "Arrow mass must be positive", [](double x){ return x > 0; });

create_constraint(settings.n_limb_elements,   "Number of limb elements must be positive",   [](int x){ return x > 0; });
create_constraint(settings.n_string_elements, "Number of string elements must be positive", [](int x){ return x > 0; });
create_constraint(settings.n_draw_steps,      "Number of draw steps must be positive",      [](int x){ return x > 0; });
create_constraint(settings.time_span_factor,  "Time span factor must be positive",          [](double x){ return x > 0; });
create_constraint(settings.time_step_factor,  "Time step factor must be between 0 and 1",   [](double x){ return x > 0.0 && x < 1.0; });
create_constraint(settings.sampling_rate,     "Sampling rate must be positive",             [](double x){ return x > 0; });

create_constraint(width, "Supplied widths must be positive", [](const Series& width)
{
    for(double w: width.vals()) {
        if(w <= 0)
            return false;
    }

    return true;
});

create_constraint(height, "Supplied heights must be positive", [](const Series& height) {
    for(double h: height.vals()) {
        if(h <= 0)
            return false;
    }

    return true;
});

create_constraint(width, "Minimum width must be positive", [](const Series& width)
{
    try
    {
        Series output = CubicSpline::sample(width, 150);    // Magic number
        for(double w: output.vals()) {
            if(w <= 0)
                return false;
        }
    }
    catch(...)
    {
        // Carry on, don't treat other problems here
    }

    return true;
});

create_constraint(height, "Minimum height must be positive", [](const Series& width)
{
    try
    {
        Series output = CubicSpline::sample(width, 150);    // Magic number
        for(double h: output.vals()) {
            if(h <= 0)
                return false;
        }
    }
    catch(...)
    {
        // Carry on, don't treat other problems here
    }

    return true;
});

create_constraint(operation.brace_height, "Brace height too low for current limb profile",
                  [&](double brace_height)
{
    try
    {
        LimbProperties limb(*this);
        for(size_t i = 0; i < limb.y.size(); ++i) {
            if(limb.y[i] <= -brace_height)
                return false;
        }
    }
    catch(...)
    {
        // Carry on, don't treat other problems here
    }

    return true;
});
*/
