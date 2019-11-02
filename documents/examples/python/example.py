import json               # Loading and saving input files
import msgpack            # Loading output files
import numpy              # Evaluating stresses
import subprocess         # Runnig the simulation

# Load input file
with open("input.bow", "r") as file:
    input = json.load(file)

# Modify input
input["string"]["n_strands"] += 1

# Save modified input
with open("input.bow", "w") as file:
    json.dump(input, file)

# Run a static simulation
subprocess.call(["virtualbow", "input.bow", "output.dat", "--static"])

# Load the output file
with open("output.dat", "rb") as file:
    output = msgpack.unpack(file, raw=False)

# Calculate maximum stress for layer 0 at full draw
He_back = numpy.array(output["limb_properties"]["layers"][0]["He_back"])
Hk_back = numpy.array(output["limb_properties"]["layers"][0]["Hk_back"])
epsilon = numpy.array(output["statics"]["states"]["epsilon"][-1])
kappa   = numpy.array(output["statics"]["states"]["kappa"][-1])

sigma_back = He_back.dot(epsilon) + Hk_back.dot(kappa)
print(sigma_back.max()) 
