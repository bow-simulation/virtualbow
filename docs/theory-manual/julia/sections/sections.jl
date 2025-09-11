include("utils.jl")

using GXBeam
using PyPlot

ash    = iso_material(12e9, 0.3, 675.0)        # https://www.wood-database.com/white-ash/
bamboo = iso_material(18e9, 0.3, 850.0)        # https://www.wood-database.com/bamboo/
glass  = iso_material(44.5e9, 0.25, 2000.0)    # https://de.wikipedia.org/wiki/Glasfaserverst%C3%A4rkter_Kunststoff
carbon = iso_material(140e9, 0.26, 1500.0)     # https://de.wikipedia.org/wiki/Kohlenstofffaserverst%C3%A4rkter_Kunststoff

# Section 1: Single layer
let
    w = 0.03    # Section width
    h = 0.01    # Section height

    for y in [0 h/2 h]
        C, M, strain_eval, stress_eval = eval_layered_section(w, [0, h] .- y, [ash], 25)

        println("Single Layer, Reference: ", y)
        println("Stiffness = ", C)
        println("Mass = ", M)
        println("Strain: ", strain_eval)
        println("Stress: ", stress_eval)
        println()
    end
end

# Section 2: Multiple layers
let
    w = 0.03    # Section width
    h = [0.0008, 0.001, 0.002, 0.001, 0.0008]    # Layer heights

    bounds = [0; cumsum(h)]
    offsets = vcat([[bounds[i], (bounds[i] + bounds[i+1])/2] for i in 1:length(bounds)-1]..., bounds[end])
    
    for y in offsets
        C, M, strain_eval, stress_eval = eval_layered_section(w, bounds .- y, [glass, bamboo, ash, bamboo, glass], 25)

        println("Multi Layer, Reference: ", y)
        println("Stiffness = ", C)
        println("Mass = ", M)
        println("Strain: ", strain_eval)
        println("Stress: ", stress_eval)
        println()
    end
end