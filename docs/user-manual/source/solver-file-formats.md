# File Formats

## Input

The input files of the solver are `.bow` model files as created by the model editor.
Those files use the text based JSON[^1] format.
JSON (JavaScript Object Notation) is a very common exchange format for hierarchical data in the form of objects, arrays, strings, numbers and more.
Since the format is text based, it is easy to inspect and modify with a text editor.

A detailed specification of the model files can be found in [Appendix A](appendix-model-files.md).

## Output

The output files of the solver are `.res` result files that can be opened with the result viewer.
The result files use the binary MessagePack[^2] format.
MessagePack is very similar to JSON in the kind of data it can represent, but more space efficient due to being a binary format.
Unlike the input files, the output files cannot be inspected with a text editor.

A detailed specification of the model files can be found in [Appendix B](appendix-result-files.md).

[^1]: [https://www.json.org/](https://www.json.org/)

[^2]: [https://msgpack.org/](https://msgpack.org/)