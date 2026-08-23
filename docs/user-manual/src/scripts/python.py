import json, msgpack      # Loading and saving model and result files
import numpy as np        # Evaluating stresses
import subprocess         # Runnig the simulation

# Load model file
with open("input.bow", "r") as file:
    input = json.load(file)

# Modify model data
input["string"]["n_strands"] += 1

# Save model file
with open("input.bow", "w") as file:
    json.dump(input, file, indent=2)

# Run a static simulation
subprocess.call(["virtualbow-cli", "static", "input.bow", "output.res"])

# Load the result file
with open("output.res", "rb") as file:
    output = msgpack.unpack(file, raw=False)

# Evaluate final draw force
print(output["statics"]["final_draw_force"])
