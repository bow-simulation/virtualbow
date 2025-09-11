include("utils.jl")

# Compare rectangular cross section implementation against cross sectional analysis

material = iso_material(18e9, 0.3, 850.0)
rectangle = Rectangle(0.03, 0.01, 0.0025)

S_ana = compliance_matrix_rect(rectangle, material)
C_ana = reduce_to_planar(inv(S_ana))

M_ana = mass_matrix_rect(rectangle, material)
M_ana = reduce_to_planar(M_ana)

display(M_ana)
display(M_num)

C_num, M_num, _, _ = eval_layered_section(rectangle.w, [0, rectangle.h] .- rectangle.y, [material], 50)

display(C_ana)
display(C_num)
