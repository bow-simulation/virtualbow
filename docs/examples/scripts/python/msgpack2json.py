import json
import msgpack
import sys

with open(sys.argv[1], "rb") as file:
    data = msgpack.unpack(file, raw=False)

with open(sys.argv[2], "w") as file:
    json.dump(data, file, indent=2)