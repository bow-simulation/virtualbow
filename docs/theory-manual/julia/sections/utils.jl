using GXBeam
using StaticArrays

# Indices of normal strain, shear strain and bending curvature of our planar beam in GXBeam's 3d coordinates
const iε = 1
const iγ = 2
const iκ = 5

# Layered, rectangular cross section from width in x direction, layer bounds in y direction and materials
function eval_layered_section(width, bounds, materials, n = 25)
    # Create a layered cross section with only one element in x direction since we only care about the planar case
    nodes, elements = layered_section(width, bounds, materials, 2, n)

    # Compute section compliance- and mass matrix
    cache = initialize_cache(nodes, elements)
    S, _, _ = compliance_matrix(nodes, elements, cache=cache, shear_center=false)
    M, _ = mass_matrix(nodes, elements)

    # Compute 3x3 stiffness matrix and 3x3 mass matrix
    K = reduce_to_planar(inv(S))
    M = reduce_to_planar(M)

    # Evaluate normal strains and stresses at the bounds of each layer
    function eval_strains(F, M)
        layer_strain = []
        layer_stress = []

        for i in 1:length(bounds)-1
            j1 = (n-1)*(i-1) + 1           # First element of the layer
            j2 = (n-1)*i                   # Last element of the layer
            h = bounds[i+1] - bounds[i]    # Layer height
            dh = h/(n-1)                   # Element height

            # Evaluate strains and stresses for first and last element
            strain, stress = strain_recovery(F, M, nodes, [elements[j1], elements[j2]], cache)

            # Extrapolate strains at layer bounds
            slope = (strain[iε,2] - strain[iε,1])/(h - dh)
            push!(layer_strain, strain[iε,1] - dh/2*slope)
            push!(layer_strain, strain[iε,2] + dh/2*slope)

            # Extrapolate stresses at layer bounds
            slope = (stress[iε,2] - stress[iε,1])/(h - dh)
            push!(layer_stress, stress[iε,1] - dh/2*slope)
            push!(layer_stress, stress[iε,2] + dh/2*slope)
        end

        return layer_strain, layer_stress
    end

    strain_N, stress_N = eval_strains([1.0 0.0 0.0], [0.0 0.0 0.0])    # Strains and stresses for a unit normal force
    strain_Q, stress_Q = eval_strains([0.0 1.0 0.0], [0.0 0.0 0.0])    # Strains and stresses for a unit shear force
    strain_M, stress_M = eval_strains([0.0 0.0 0.0], [0.0 1.0 0.0])    # Strains and stresses for a unit bending moment

    # Concatenating the unit stresses results in the evaluation matrices that recover stresses and strains
    # when multiplied by the vector of cross section forces [N, Q, M]
    strain_eval = hcat(strain_N, strain_Q, strain_M)
    stress_eval = hcat(stress_N, stress_Q, stress_M)

    return K, M, strain_eval, stress_eval
end

# Layered, rectangular cross section from width in x direction, layer bounds in y direction and materials
function layered_section(width, bounds, materials, nx = 25, ny = 25)
    @assert all(diff(bounds) .> 0)
    @assert length(materials) == length(bounds) - 1

    nodes = []
    elements = []

    # Grid points in x direction for the section, i.e. all layers
    x_grid = range(-width/2, width/2, length=nx)

    # For the first layer bound, create a single row of nodes
    for ix in 1:nx
        push!(nodes, Node(x_grid[ix], bounds[1]))
    end

    # For each subsequent layer bound create a layer between the current and previous one
    for ib in 2:length(bounds)
        # Grid points in y direction for the layer
        y_grid = range(bounds[ib-1], bounds[ib], length=ny)

        for iy in 2:ny
            # Create a new row of nodes
            for ix in 1:nx
                push!(nodes, Node(x_grid[ix], y_grid[iy]))
            end

            # Create new elements between the current and the last row
            for ix in 1:nx-1
                push!(elements, MeshElement([length(nodes) - 2*nx + ix + 1, length(nodes) - nx + ix + 1, length(nodes) - nx + ix, length(nodes) - 2*nx + ix], materials[ib-1], 0.0))
            end
        end
    end

    return nodes, elements
end

# Isotropic material from young's modulus, poisson ratio and density
function iso_material(E, v, ρ)
    G = E/(2*(1 + v))
    return Material(E, E, E, G, G, G, v, v, v, ρ)
end

# Reduces a 6x6 matrix for the 3d beam to a 3x3 matrix for a planar beam
# The components of the resulting matrix are {epsilon, gamma, kappa}
function reduce_to_planar(A)
    return [
        A[iε, iε] A[iε, iγ] A[iε, iκ]
        A[iγ, iε] A[iγ, iγ] A[iγ, iκ]
        A[iκ, iε] A[iκ, iγ] A[iκ, iκ]
    ]
end

# Extends a 3x3 matrix for a planar beam to a 6x6 matrix for the 3d beam
function extend_from_planar(A)
    # Bending and shear are repeated for the out-of-plane components
    # Torsional component is set to 1, since we don't know it but also don't care for a planar beam
    return [
        A[1,1] A[1,2]     0  0 A[1,3]      0
        A[2,1] A[2,2]     0  0 A[2,3]      0
            0      0 A[2,2]  0      0      0
            0      0      0  1      0      0
        A[3,1] A[2,3]     0  0 A[3,3]      0
            0      0      0  0      0 A[3,3]
   ]
end

struct Rectangle
    w::Float64    # Width
    h::Float64    # Height
    y::Float64    # Offset of center from bottom
end

# Compliance matrix for a rectangular cross section of a planar beam
function compliance_matrix_rect(rectangle::Rectangle, material::Material)
    E = material.E1
    G = material.G12

    e = rectangle.y - rectangle.h/2
    A = rectangle.w*rectangle.h
    I = 1/12*rectangle.w*rectangle.h^3 + A*e^2

    EA  = E*A
    EI  = E*I
    GA  = G*A

    C = [EA  0 -EA*e; 0 GA 0; -EA*e 0  EI]
    C = extend_from_planar(C)
    S = inv(C)

    return S
end

# Mass matrix for a rectangular cross section of a planar beam
# (Currently with respect to the center of mass)
function mass_matrix_rect(rectangle::Rectangle, material::Material)
    ρ = material.rho
    e = rectangle.y - rectangle.h/2
    A = rectangle.w*rectangle.h
    I = 1/12*rectangle.w*rectangle.h^3 # + A*e^2

    ρA  = ρ*A
    ρI  = ρ*I

    M = [ρA  0 -ρA*e; 0 ρA 0; -ρA*e 0  ρI]
    M = extend_from_planar(M)

    return M
end

# Linear interpolation between two rectangle sections
function interpolate(r1::Rectangle, r2::Rectangle, n::Float64)
    w = interpolate(r1.w, r2.w, n)
    h = interpolate(r1.h, r2.h, n)
    y = interpolate(r1.y, r2.y, n)

    return Rectangle(w, h, y)
end

# Cubic interpolation between four rectangle sections
function interpolate(r1::Rectangle, r2::Rectangle, r3::Rectangle, r4::Rectangle, n::Float64)
    w = interpolate(r1.w, r2.w, r3.w, r4.w, n)
    h = interpolate(r1.h, r2.h, r3.h, r4.h, n)
    y = interpolate(r1.y, r2.y, r3.y, r4.y, n)

    return Rectangle(w, h, y)
end

# Linear interpolation between equidistant values y1 and y2, parameter n in [0, 1]
function interpolate(y1, y2, n)
    return (1 - n)*y1 + n*y2
end

# Cubic interpolation between equidistant values y1, y2, y3 and y4, parameter n in [0, 1]
function interpolate(y1, y2, y3, y4, n)
    c0 = y1
    c1 = (2*y4 - 9*y3 + 18*y2 - 11*y1)/2
    c2 = (-9*y4 + 36*y3 - 45*y2 + 18*y1)/2
    c3 = (9*y4 - 27*y3 + 27*y2 - 9*y1)/2

    return c0 + c1*n + c2*n^2 + c3*n^3
end