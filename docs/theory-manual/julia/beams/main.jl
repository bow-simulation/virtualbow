include("simulate.jl")

using MsgPack

MsgPack.msgpack_type(::Type{Output}) = MsgPack.StructType()
MsgPack.msgpack_type(::Type{OutputState}) = MsgPack.StructType()
MsgPack.msgpack_type(::Type{Settings}) = MsgPack.StructType()

Fx = -60       # Force in x direction
Fy = 80        # Force in y direction
Mz = 10        # Torque around z axis
omega = 200    # Frequency of dynamic Load

settings = Settings(
    n_elements = 1000,      # Number of elements
    n_eigen = 12,           # Number of eigenvalues (2 eigenvalues = 1 frequency)
    n_static = 10,          # Number of static states being simulated, including zero and full load
    n_dynamic = 250,        # Number of dynamic states being simulated, including zero and final time
    n_dynamic_out = 10      # Number of dynamic states written to output
)

# Constructs one of several possible curves by name
function create_curve(name)
    # Straight line
    if name == "curve1"
        return 0.9, line(0.9)
    end

    # Circular arc
    if name == "curve2"
        return 0.9, arc(0.9, 1.2)
    end

    # Circular waves
    if name == "curve3"
        return 0.9, waves(0.9, 10)
    end

    error("Unknown curve: " * name)
end

# Constructs one of several cross sections by name
function create_section(name)
    # Constant, center as reference
    if name == "section1"
        return cross_section(800.0, 15e9, 5e9, [0.01], [0.01], [0.0])
    end

    # Constant, bottom as reference
    if name == "section2"
        return cross_section(800.0, 15e9, 5e9, [0.01], [0.01], [-0.005])
    end

    # Constant, top as reference
    if name == "section3"
        return cross_section(800.0, 15e9, 5e9, [0.01], [0.01], [0.005])
    end

    # Linear, center as reference
    if name == "section4"
        return cross_section(800.0, 15e9, 5e9, [0.012, 0.006], [0.012, 0.006], [0.0, 0.0])
    end

    # Linear, changing reference
    if name == "section5"
        return cross_section(800.0, 15e9, 5e9, [0.012, 0.006], [0.012, 0.006], [-0.006, 0.003])
    end

    # Cubic, center as reference
    if name == "section6"
        return cross_section(800.0, 15e9, 5e9, [0.012, 0.009, 0.012, 0.009], [0.012, 0.009, 0.012, 0.009], [0.0, 0.0, 0.0, 0.0])
    end

    # Cubic, changing reference
    if name == "section7"
        return cross_section(800.0, 15e9, 5e9, [0.012, 0.009, 0.012, 0.009], [0.012, 0.009, 0.012, 0.009], [-0.006, 0.0045, -0.006, 0.0045])
    end

    error("Unknown section: " * name)
end

path = mkpath("gxbeam/")

# Simulate all combinations of curves and cross sections
# and save the results as a file for each combination.
for curve_name in ["curve1", "curve2", "curve3"]
    L, curve = create_curve(curve_name)

    for section_name in ["section1", "section2", "section3", "section4", "section5", "section6", "section7"]
        filename = curve_name * "_" * section_name * ".msgpack"
        filepath = path * "/" * filename
        println(filename, "...")

        if isfile(filepath)
            continue
        end

        compliance, mass = create_section(section_name)        
        assembly = create_assembly(L, settings.n_elements, curve, compliance, mass)
        output = simulate_assembly(assembly, Fx, Fy, Mz, omega, settings)

        open(filepath, "w") do io
            pack(io, output)
        end
    end
end
