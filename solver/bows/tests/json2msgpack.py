import json
import msgpack
import sys

with open(sys.argv[1], "r") as file:
    data = json.load(file)

with open(sys.argv[2], "wb") as file:
    file.write(msgpack.packb(data))