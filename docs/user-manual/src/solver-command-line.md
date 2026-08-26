# Command Line

The VirtualBow executables are named

```
virtualbow-gui        // Model editor
virtualbow-cli        // Solver
```

The solver executable takes a `.bow` model file as its input, performs the simulation and saves the results as a `.res` file.
The type of simulation (static or dynamic) as well as other options are set by the command line arguments.
The detailed usage as shown by the `--help` option is:

```
Usage: virtualbow_cli [OPTIONS] <MODE> <INPUT> <OUTPUT>

Arguments:
  <MODE>    Mode of simulation [possible values: static, dynamic]
  <INPUT>   Input model file (*.bow)
  <OUTPUT>  Output result file (*.res)

Options:
  -p, --progress  Print simulation progress
  -v, --version   Print version
  -h, --help      Print help
```

> [!NOTE]
> To use the command line interface on Windows, you have to either specify the complete path to the respective executable or add the installation directory to your `PATH` environment variable.
> There is an option to do this automatically during installation of VirtualBow.

> [!NOTE]
> On macOS, the VirtualBow executables are hidden inside the application bundle.
> They can be accessed by their full path though, or their location can be temporarily added to the `PATH` environment variable with the command `export PATH=$PATH:/Applications/VirtualBow.app/Contents/MacOS`.
> Put this line into your shell's startup file (`.zshrc` for the default zsh, `.bash_profile` for bash) if you want it to be added permanently.
