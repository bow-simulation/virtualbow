using GXBeam

# Compliance- and mass function for constant cross section
function cross_section(ρ::Float64, E::Float64, G::Float64, w::Vector{Float64}, h::Vector{Float64}, y::Vector{Float64})
    function compliance(n)
        return eval_compliance_matrix(E, G, interpolate(w, n), interpolate(h, n), interpolate(y, n))
    end

    function mass(n)
        return eval_mass_matrix(ρ, interpolate(w, n), interpolate(h, n), interpolate(y, n))
    end

    return compliance, mass
end

# Compliance matrix for a rectangular cross section of a planar beam
function eval_compliance_matrix(E::Float64, G::Float64, w::Float64, h::Float64, y::Float64)
    A = w*h
    I = (w*h^3)/12 + A*y^2

    EA  = E*A
    GA  = G*A
    EI  = E*I
    
    # Stiffness matrix for planar beam.
    # Entries we don't care about are set to 1, so that it can be inverted to get the compliance matrix.
    return inv([
           EA  0 0 0 0 -EA*y
            0 GA 0 0 0     0
            0  0 1 0 0     0
            0  0 0 1 0     0
            0  0 0 0 1     0
        -EA*y  0 0 0 0    EI
    ])
end

# Mass matrix for a rectangular cross section of a planar beam
# (Currently with respect to the center of mass)
function eval_mass_matrix(ρ::Float64, w::Float64, h::Float64, y::Float64)
    A = w*h
    I = (w*h^3)/12 + A*y^2

    ρA  = ρ*A
    ρI  = ρ*I

    # Mass matrix for planar beam.
    # Entries we don't care about are set to 1.
    return [
        ρA  0 0 0 0 -ρA*y
         0 ρA 0 0 0     0
         0  0 1 0 0     0
         0  0 0 1 0     0
         0  0 0 0 1     0
     -ρA*y  0 0 0 0    ρI
    ]
end

function interpolate(y::Vector{Float64}, n)
    if length(y) == 1
        return y[1]
    end

    if length(y) == 2
        return (1 - n)*y[1] + n*y[2]
    end

    if length(y) == 4
        c0 = y[1]
        c1 = (2*y[4] - 9*y[3] + 18*y[2] - 11*y[1])/2
        c2 = (-9*y[4] + 36*y[3] - 45*y[2] + 18*y[1])/2
        c3 = (9*y[4] - 27*y[3] + 27*y[2] - 9*y[1])/2
        
        return c0 + c1*n + c2*n^2 + c3*n^3
    end

    error("Invalid length: " * length(y))
end

# Linear interpolation between equidistant values y1 and y2, parameter n in [0, 1]
#function interpolate(y1, y2, n)
#    return (1 - n)*y[1] + n*y[2]
#end

# Cubic interpolation between equidistant values y1, y2, y3 and y4, parameter n in [0, 1]
#function interpolate(y1, y2, y3, y4, n)
#    c0 = y1
#    c1 = (2*y4 - 9*y3 + 18*y2 - 11*y1)/2
#    c2 = (-9*y4 + 36*y3 - 45*y2 + 18*y1)/2
#    c3 = (9*y4 - 27*y3 + 27*y2 - 9*y1)/2
#
#    return c0 + c1*n + c2*n^2 + c3*n^3
#end