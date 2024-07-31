# Scripting

The main use of the solver as a standalone application is the ability to write programs that automatically launch simulations and analyze the results.
This is especially useful for performing design optimizations or exploring the influence of certain model parameters on the simulation results.

Scripts that interoperate with VirtualBow have to be able to call the solver via the command line interface as well as read and write the model and result files that constitute the input and output of the solver.
As a consequence of the file formats, any programming language that supports JSON and MessagePack can be used.
Both are fairly common formats and most languages have either built-in support or there are external libraries available.

For getting started, the following sections show how to interface with VirtualBow for some programming languages commonly used in scientific computing.
Each of the examples performs the same series of basic tasks:

1. Load, modify and save a model file
2. Run a static simulation with the model file
3. Load the result file and evaluate the maximum stress of the first layer at full draw

## Python

The python example below uses two external packages, [msgpack](https://pypi.org/project/msgpack/) for reading the result files and [numpy](https://pypi.org/project/numpy/) for evaluating the stresses.
They can be installed with `pip install msgpack numpy`.

```python
{{#include scripts/python.py}}
```

## Matlab

This Matlab example uses the [JSONLab](https://de.mathworks.com/matlabcentral/fileexchange/33381) library, which can read and write both JSON and MessagePack files.

```matlab
{{#include scripts/matlab.m}}
```

## Julia

For the Julia example, two external packages are used: [JSON](https://juliahub.com/ui/Packages/JSON/uf6oy) for loading model files and [MsgPack](https://juliahub.com/ui/Packages/MsgPack/oDgLV) for loading result files.
They can be installed with `julia> import Pkg; Pkg.add("JSON"); Pkg.add("MsgPack")`.

```julia
{{#include scripts/julia.jl}}
```