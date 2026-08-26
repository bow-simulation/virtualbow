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
run(`virtualbow-cli static input.bow output.res`)

# Load the result file
stream = open("output.res", "r")
output = unpack(stream)
close(stream)

# Evaluate final draw force
println(output["statics"]["final_draw_force"])
