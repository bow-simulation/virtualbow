import msgpack

# Read msgpack file
with open('file.dat') as file:
    data = msgpack.unpack(file)
    
print(data)
