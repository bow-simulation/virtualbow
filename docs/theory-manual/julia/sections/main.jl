include("utils.jl")

# Material 1
E = 100.0
v = 0.3
ρ = 1000.0

G = E/(2*(1 + v))
mat1 = Material(E, E, E, G, G, G, v, v, v, ρ)

w = 0.5
y = [-0.3, -0.25, 0.25, 0.3]

nodes, elements = layered_section(w, y, [mat1, mat1, mat1])

using PyPlot

figure();
plotmesh(nodes, elements, PyPlot)
axis("equal")
display(gcf())