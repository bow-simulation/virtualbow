# File Formats

## Model files

The solver reads `.bow` model files as created by the VirtualBow model editor.
These files use the text-based [JSON](https://www.json.org/) format.
JSON (JavaScript Object Notation) is a widely used standard for hierarchical data composed of objects, arrays, strings, numbers, and more.
Because it is human-readable, `.bow` files can be easily inspected or modified with any text editor.

## Result files

The solver produces `.res` result files containing all static and/or dynamic simulation results.
These files use the binary [MessagePack](https://msgpack.org/) format.
MessagePack represents the same kinds of data as JSON but in a more compact binary form.
Unlike the input files, `.res` files cannot be viewed directly in a text editor, but *Visual Studio Code*, for example, can display them with a plugin such as `PackLens` or `Data File Viewer`.

> [!NOTE]
> Earlier versions of this manual included detailed specifications of the internal structure of both input and output files.
> As the contents of these files have grown and evolved, maintaining those specifications has become impractical.
> Fortunately, their content is not too complicated.
> Most of it can be understood by experimenting and examining the resulting files.
> The field names often correspond to the names used in the VirtualBow GUI.
> If you have questions about the file formats or need guidance for advanced use cases, feel free to get in touch.

> [!WARNING]
> The internal file structure used by VirtualBow changes frequently.
> VirtualBow maintains backward compatibility only for `.bow` model files, not for `.res` result files.
> If you integrate tools or workflows with these formats, you should expect to have to update them to keep them working with newer versions of VirtualBow.
