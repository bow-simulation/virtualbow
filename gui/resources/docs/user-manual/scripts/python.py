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
subprocess.call(["virtualbow-slv", "--static", "input.bow", "output.res"])

# Load the result file
with open("output.res", "rb") as file:
    output = msgpack.unpack(file, raw=False)

# Evaluate stresses
He_back = np.array(output["setup"]["limb_properties"]["layers"][0]["He_back"])
Hk_back = np.array(output["setup"]["limb_properties"]["layers"][0]["Hk_back"])

epsilon = np.array(output["statics"]["states"]["epsilon"][-1])
kappa   = np.array(output["statics"]["states"]["kappa"][-1])
sigma = He_back.dot(epsilon) + Hk_back.dot(kappa)

print(sigma.max())
