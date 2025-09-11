include("sections.jl")
include("curves.jl")

using GXBeam

Base.@kwdef struct OutputState
    x::Vector{Float64} = []
    y::Vector{Float64} = []
    phi::Vector{Float64} = []

    N::Vector{Float64} = []
    Q::Vector{Float64} = []
    M::Vector{Float64} = []

    epsilon::Vector{Float64} = []
    gamma::Vector{Float64} = []
    kappa::Vector{Float64} = []
end

function OutputState(assembly::Assembly, state::AssemblyState{<:Any, <:Any, <:Any})
    # Approximate initial rotation angles from the points of the assembly
    # TODO: Is there a way to get the exact values back here?
    dx = diff(getindex.(assembly.points, 1))
    dy = diff(getindex.(assembly.points, 2))
    initial_angles = element_to_nodes(atan.(dy, dx))

    # Compute strains from forces and element compliance matrices
    forces = [[state.elements[i].Fi; state.elements[i].Mi] for i = 1:length(assembly.elements)]
    strains = [assembly.elements[i].compliance*forces[i] for i = 1:length(assembly.elements)]

    return OutputState(
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

Base.@kwdef struct Output
    eigen::Vector{Tuple{Float64, Float64}} = []
    statics::Vector{OutputState} = []
    dynamics::Vector{OutputState} = []
end

function element_to_nodes(values)
    averages = [(values[i] + values[i+1])/2 for i = 1:length(values)-1]
    return [values[1]; averages; values[end]]
end

function simulate_assembly(assembly::Assembly, Fx, Fy, Mz)
    # 1. Eigenvalues

    prescribed_conditions = Dict(1 => PrescribedConditions(ux=0, uy=0, uz=0, theta_x=0, theta_y=0, theta_z=0))    # Fixed left end
    _, lambda, _, converged = eigenvalue_analysis(assembly, prescribed_conditions=prescribed_conditions, nev=12, linear=true, two_dimensional=true)
    @assert converged
    eigen = [(real(x), imag(x)) for x in lambda]

    # 2. Statics

    λ = 0:0.1:1.0    # Load scaling factor   
    statics = Vector{OutputState}(undef, length(λ))    # One output state for each scaling

    for i = 1:length(λ)
        # Create dictionary of prescribed conditions
        prescribed_conditions = Dict(
            1 => PrescribedConditions(ux=0, uy=0, uz=0, theta_x=0, theta_y=0, theta_z=0),    # Fixed left end
            N+1 => PrescribedConditions(Fx=λ[i]*Fx, Fy=λ[i]*Fy, Mz=λ[i]*Mz)                  # Forces on the right end
        )

        # Perform a static analysis, make sure it converged
        _, state, converged = static_analysis(assembly, prescribed_conditions=prescribed_conditions)
        @assert converged

        statics[i] = OutputState(assembly, state)
    end

    return Output(eigen=eigen, statics=statics)
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