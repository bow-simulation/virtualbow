# Command Line

The VirtualBow executables are named

```
virtualbow-gui        // Model editor
virtualbow-slv        // Solver
virtualbow-post       // Result viewer
```

The solver executable takes a `.bow` model file as its input, performs the simulation and saves the results as a `.res` file that can be opened by the result viewer.
The type of simulation (static or dynamic) as well as other options are set by the command line arguments.
The detailed usage as shown by the `--help` option is:

Usage: virtualbow-slv [options] input output

```
Options:
  -?, -h, --help  Displays this help.
  -v, --version   Displays version information.
  -s, --static    Run a static simulation.
  -d, --dynamic   Run a dynamic simulation.
  -p, --progress  Print simulation progress.

Arguments:
  input           Model file (.bow)
  output          Result file (.res)
```

NOTE: To use the command line interfaces on Windows you have to either specify the complete path to the respective executable or add the installation directory to your `PATH` environment variable.
There is an option to do this automatically during installation of VirtualBow.

NOTE: On MacOS, the VirtualBow executables are hidden inside the application bundle.
They can be accessed by their full path though, or their location can be temporarily added to the `PATH` environment variable with the command `export PATH = $PATH:/Applications/VirtualBow.app/Contents/MacOS`.
Put this line into your `.bash_profile` if you want it to be permanently added.