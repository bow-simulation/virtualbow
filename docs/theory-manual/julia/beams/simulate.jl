include("sections.jl")
include("curves.jl")

using GXBeam

struct Settings
    n_elements::Int64
    n_eigen::Int64
    n_static::Int64
    n_dynamic::Int64
    i_dynamic::Vector{Int64}
end

function Settings(; n_elements::Int64, n_eigen::Int64, n_static::Int64, n_dynamic::Int64, n_dynamic_out::Int64)
    return Settings(
        n_elements,
        n_eigen,
        n_static,
        n_dynamic,
        round.(Int, range(1, stop=n_dynamic, length=n_dynamic_out))
    )
end

Base.@kwdef struct OutputState
    lambda::Float64
    time::Float64

    x::Vector{Float64}
    y::Vector{Float64}
    phi::Vector{Float64}

    N::Vector{Float64}
    Q::Vector{Float64}
    M::Vector{Float64}

    epsilon::Vector{Float64}
    gamma::Vector{Float64}
    kappa::Vector{Float64}
end

Base.@kwdef struct Output
    settings::Settings = []
    eigen::Vector{Tuple{Float64, Float64}} = []
    statics::Vector{OutputState} = []
    dynamics::Vector{OutputState} = []
end

function OutputState(assembly::Assembly, state::AssemblyState{<:Any, <:Any, <:Any}, lambda::Float64, time::Float64)
    function element_to_nodes(values)
        averages = [(values[i] + values[i+1])/2 for i = 1:length(values)-1]
        return [values[1]; averages; values[end]]
    end

    # Approximate initial rotation angles from the points of the assembly
    # TODO: Is there a way to get the exact values back here?
    dx = diff(getindex.(assembly.points, 1))
    dy = diff(getindex.(assembly.points, 2))
    initial_angles = element_to_nodes(atan.(dy, dx))

    # Compute strains from forces and element compliance matrices
    forces = [[state.elements[i].Fi; state.elements[i].Mi] for i = 1:length(assembly.elements)]
    strains = [assembly.elements[i].compliance*forces[i] for i = 1:length(assembly.elements)]

    return OutputState(
        lambda = lambda,    
        time = time,

        x   = [assembly.points[i][1] + state.points[i].u[1] for i = 1:length(assembly.points)],
        y   = [assembly.points[i][2] + state.points[i].u[2] for i = 1:length(assembly.points)],
        phi = [initial_angles[i] + 4*atan(state.points[i].theta[3]/4) for i = 1:length(assembly.points)],    # https://github.com/byuflowlab/GXBeam.jl/issues/69

        N = [state.elements[i].Fi[1] for i = 1:length(assembly.elements)],
        Q = [state.elements[i].Fi[2] for i = 1:length(assembly.elements)],
        M = [state.elements[i].Mi[3] for i = 1:length(assembly.elements)],

        epsilon = getindex.(strains, 1),
        gamma = getindex.(strains, 2),
        kappa = getindex.(strains, 6),
    )
end

function simulate_assembly(assembly::Assembly, Fx, Fy, Mz, omega, settings::Settings)
    # 1. Eigenvalues
    println("Eigenvalues")

    eigen_conditions = Dict(1 => PrescribedConditions(ux=0, uy=0, uz=0, theta_x=0, theta_y=0, theta_z=0))    # Fixed left end
    _, eigenvalues, _, converged = eigenvalue_analysis(assembly, prescribed_conditions=eigen_conditions, nev=settings.n_eigen, linear=true, two_dimensional=true)
    @assert converged
    eigen = [(real(x), imag(x)) for x in eigenvalues]

    # 2. Statics
    println("Statics")

    lambda = range(0, stop=1, length=settings.n_static)    # Load scaling factor   
    statics = Vector{OutputState}(undef, settings.n_static)    # One output state for each scaling
    system = StaticSystem(assembly)

    for i = 1:length(lambda)
        # Create dictionary of prescribed conditions
        static_conditions = Dict(
            1 => PrescribedConditions(ux=0, uy=0, uz=0, theta_x=0, theta_y=0, theta_z=0),    # Fixed left end
            settings.n_elements + 1 => PrescribedConditions(Fx=lambda[i]*Fx, Fy=lambda[i]*Fy, Mz=lambda[i]*Mz)    # Scaled forces on the right end
        )

        # Perform a static analysis, make sure it converged
        _, state, converged = static_analysis!(system, assembly, prescribed_conditions=static_conditions, reset_state=false)
        @assert converged

        # Save state to output
        statics[i] = OutputState(assembly, state, lambda[i], 0.0)
    end

    # 3. dynamics
    println("Dynamics")

    period = 2*pi/omega
    tvec = range(0, stop=period, length=settings.n_dynamic)
    save = settings.i_dynamic

    function dynamic_conditions(t)
        Dict(
            1 => PrescribedConditions(ux=0, uy=0, uz=0, theta_x=0, theta_y=0, theta_z=0),              # Fixed left end
            settings.n_elements + 1 => PrescribedConditions(Fx=sin(omega*t)*Fx, Fy=sin(omega*t)*Fy, Mz=sin(omega*t)*Mz)    # Dynamic forces on the right end
        )
    end

    _, history, converged = time_domain_analysis(assembly, tvec, save=save, prescribed_conditions=dynamic_conditions, structural_damping=false, reset_state=true)
    @assert converged
    dynamics = [OutputState(assembly, history[i], 0.0, tvec[save[i]]) for i = 1:length(save)]

    return Output(settings=settings, eigen=eigen, statics=statics, dynamics=dynamics)
end

# curve: [0, 1] -> [x, y, φ]
function create_assembly(L, N, curve::Function, compliance::Function, mass::Function)
    # Evaluating properties at element start- and endpoints
    n = range(0, 1, length=N+1)
    s = n*L
    p = curve.(n)
    points = map(v -> [v[1], v[2], 0.0], p)
    frames = map(v -> [cos(v[3]) -sin(v[3]) 0.0; sin(v[3]) cos(v[3]) 0; 0.0 0.0 1], p[1:end-1])
    lengths = diff(s)

    # Evaluating properties at element midpoints
    n = (n[1:end-1] .+ n[2:end])./2
    p = curve.(n)
    midpoints = map(v -> [v[1], v[2], 0.0], p)
    compliance = compliance.(n)    # Evaluate compliance matrices at midpoints
    mass = mass.(n)    # Evaluate mass matrices at midpoints

    # Set damping to zero (has a nonzero default otherwise)
    damping = fill(zeros(6), N)

    # Index of endpoints of each beam element
    start = 1:N
    stop = 2:N+1
    
    # Create assembly of beam elements
    return Assembly(points, start, stop, frames=frames, lengths=lengths, midpoints=midpoints, compliance=compliance, mass=mass, damping=damping)
end
