using GXBeam
using PyPlot

function rectangular_section(xmin, xmax, ymin, ymax, material, n = 25)
    x = range(xmin, xmax, length=n)
    y = range(ymin, ymax, length=n)
    
    nodes = Vector{Node{Float64}}(undef, n*n)
    elements = Vector{MeshElement{Float64}}(undef, (n-1)*(n-1))
    
    let
        m = 1
        for i = 1:n
            for j = 1:n
                nodes[m] = Node(x[i], y[j])
                m += 1
            end
        end
    
        m = 1
        for i = 1:(n-1)
            for j = 1:(n-1)
                elements[m] = MeshElement([n*(i-1)+j, n*(i)+j, n*(i)+j+1, n*(i-1)+j+1], material, 0.0)
                m += 1
            end
        end
    end

    return nodes, elements
end

# Section width and height
w = 0.05
h = 0.05

# Isotropic material properties
rho = 1000.0
E = 10e9
v = 0.3

G = E/(2*(1 + v))
material = Material(E, E, E, G, G, G, v, v, v, rho)

# Section 1: Centered
nodes1, elements1 = rectangular_section(-w/2, w/2, -h/2, h/2, material)
M1, _ = mass_matrix(nodes1, elements1)

# Section 2: Offset by h/2
nodes2, elements2 = rectangular_section(-w/2, w/2, 0, h, material)
M2, _ = mass_matrix(nodes2, elements2)

display(M1)
display(M2)

#figure();
#plotmesh(nodes1, elements1, PyPlot)
#axis("equal")

#figure();
#plotmesh(nodes2, elements2, PyPlot)
#axis("equal")

#display(gcf())