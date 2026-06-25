# Scripting

The primary reason to use the solver as a standalone application is to automate simulations and analyze the results programmatically. This is especially useful for tasks such as design optimization, parameter sweeps, or studying how specific model parameters influence the simulation outcome.

Scripts that interact with VirtualBow must be able to launch the solver through its command‑line interface and read or write the model and result files that serve as its input and output. Because the solver uses JSON for input and MessagePack for output, any programming language with support for these formats can be used. Both are widely supported, either natively or through readily available libraries.

To help you get started, the following sections demonstrate how to interface with VirtualBow in several programming languages commonly used in scientific computing. Each example performs the same sequence of basic tasks:

1. Load, modify, and save a model file

2. Run a static simulation using that model

3. Load the result file and print the final draw force

## Python

The python example below uses the external [msgpack](https://pypi.org/project/msgpack/) package for reading the result files.
It can be installed with `pip install msgpack`.

```python
{{#include scripts/python.py}}
```

## Matlab

The Matlab example uses the [JSONLab](https://de.mathworks.com/matlabcentral/fileexchange/33381) library, which can read and write both JSON and MessagePack files.

```matlab
{{#include scripts/matlab.m}}
```

## Julia

For the Julia example, two external packages are used: [JSON](https://juliahub.com/ui/Packages/JSON/uf6oy) for loading model files and [MsgPack](https://juliahub.com/ui/Packages/MsgPack/oDgLV) for loading result files.
They can be installed with `julia> import Pkg; Pkg.add("JSON"); Pkg.add("MsgPack")`.

```julia
{{#include scripts/julia.jl}}
```
