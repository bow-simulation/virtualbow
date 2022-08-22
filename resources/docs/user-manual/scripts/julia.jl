using JSON        # Loading and saving model files
using MsgPack     # Loading result files

# Load model file
stream = open("input.bow", "r")
input = JSON.parse(stream)
close(stream)

# Modify model data
input["string"]["n_strands"] += 1

# Save model file
stream = open("input.bow", "w")
JSON.print(stream, input, 2)
close(stream)

# Run a static simulation
run(`virtualbow-slv --static input.bow output.res`)

# Load the result file
stream = open("output.res", "r")
output = unpack(stream)
close(stream)

# Evaluate stresses
He_back = hcat(output["setup"]["limb_properties"]["layers"][1]["He_back"]... )
Hk_back = hcat(output["setup"]["limb_properties"]["layers"][1]["Hk_back"]... )

epsilon = output["statics"]["states"]["epsilon"][end]
kappa   = output["statics"]["states"]["kappa"][end]
sigma = He_back*epsilon + Hk_back*kappa

println(max(sigma...))
