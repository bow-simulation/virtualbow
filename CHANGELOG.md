# Changelog

## Version 0.10.0
---

**Added**

* New material properties: shear modulus, tensile and compressive strength
* Option to change the definition of the draw length to the AMO convention
* Option to specify the arrow mass relative to the draw force or the stored energy
* Dedicated result page for arrow position, velocity and acceleration
* Dedicated result page for the force-draw curve and its stiffness
* New output results for the string tip and string center angles
* Indication of material failure based on stresses and strains
* Detailed breakdown of the energy remaining in the limbs and string and lost to damping
* Creation of backup files before converting models to a newer version
* Simulations can be run with unsaved bow models
* Option to export the 3D view of the limb(s) as an image
* More and better error messages
* Additional and improved tooltips throughout the user interface
* Verification tests for the numerical methods
* Verification tests for beam statics and dynamics
* Verification tests for many of the bow simulation outputs

**Changed**

* Complete rewrite and redesign of the solver for better accuracy, performance and robustness
* Use of implicit time integration with an adaptive step size for dynamic analysis
* More accurate computation of damping forces
* More restrictive rules for layer heights to prevent malformed geometries
* Width and height plots show arc length instead of relative position, where possible
* Layer stress results are now divided by tension and compression
* More fine-grained evaluation of stresses and strains

**Fixed**

* Memory leak on macOS

**Removed**

* Support for Intel-based macOS

## Version 0.9.1
---

**Fixed**

* Wrong unit on string length result
* Error in computation of the limb geometry that leads to unwanted sharp limb tips

## Version 0.9.0
---

**Added**

* Configurable units for all inputs and outputs (including SI and US units)
* Easier way to model the profile using line, arc, spiral and spline segments
* Option to visualize the curvature of the profile
* Option to overlay images over plots
* Plot contents can be exported as CSV files
* Input fields for numbers accept arithmetic expressions
* Better error message on failed bracing

**Changed**

* Different layout of the bow editor based on panels instead of dialogs
* Material properties are edited separately from the layer properties
* Improved monotonic cubic spline interpolation (width and height)
* Automatically convert the decimal comma to a dot for keyboard layouts that use a comma
* The number of layers is no longer limited (previously 15)
* The number of table rows is no longer limited (previously 100)

**Fixed**

* Error when saving files to a path that contains special characters
* Missing error handling when loading and saving files
* Colors in the stress plot not always easily distinguishable

## Version 0.8.0
---

**Added**

* Arrow clamp force parameter to prevent the arrow from leaving the bow too early in some cases

**Changed**

* Main windows are now empty until a file is loaded or created
* Allow layers to have zero height for modeling fadeouts
* Renamed 'storage ratio' to 'energy storage factor'
* Show minimum and maximum stresses instead of maximum absolute value
* The y-axis of the energy plot always starts at zero
* Increase resolution of images exported from plots
* New icons for the toolbar, menu actions and model tree
* Better random color generation for the layers
* Visual improvements to the limb view

**Fixed**

* Action *New File* ignored modifications to already loaded files
* Remove files from *Open Recent* menu if they no longer exist
* Wrong default selection between static/dynamic results

## Version 0.7.1
---

**Fixed**

* Missing libraries for VirtualBow Solver and Post on MacOS
* Static/Dynamic buttons in VirtualBow Post not working on MacOS
* Simulation failing with `Error: bad_function_call` for certain lengths of the profile curve

## Version 0.7.0
---

**Added**

* Separate component *VirtualBow Post* for viewing simulation results
* Separate component *VirtualBow Solver* for performing simulations
* Damping parameters for limbs and string
* Show kinetic energy of limbs and string at arrow departure
* Show mass of limbs and string
* Show maximum forces and stresses
* Help menu entry that opens the user manual
* File menu entry *Open Recent*
* Indicate in the profile, width and height plots which points are being edited
* Remember open tab and simulation mode in VirtualBow Post
* Play buttons for the output slider in VirtualBow Post
* *Go to* menu for the output slider in VirtualBow Post
* Option to add installation directory to PATH on Windows

**Changed**

* Application logo
* Compile with MinGW under Windows for better simulation performance
* The profile curve now has continuous curvature, no more jumps
* Keep the blue default color on stress plots when there is only one layer
* More ticks on the y-axis of the stress plots and others

**Fixed**

* Problem that `VCRUNTIME_140_1.dll` was not found under Windows
* Short flashing of the console window when starting VirtualBow on Windows

**Removed**

* Most of the menu icons, except very specific ones
* Releases for 32 bit Windows
* AppImage releases

## Version 0.6.1
---

**Fixed**

* Error when trying to read the simulation output on Windows with the Python example script

## Version 0.6.0
---

**Added**

* Support for MacOS
* Support for `rpm` based Linux distributions
* Output limb curvature

**Changed**

* Use monotonic cubic splines for layer width and height interpolation
* Define the offsets of the limb centerline in terms of *setback* and *handle length* parameters

**Fixed**

* Crash when loading an invalid `.bow` file from the command line
* Problem with face orientation of the limb geometry in the 3D view

## Version 0.5.0
---

**Added**

* Possibility to define laminated bows
* Support for 64 bit Windows
* Cancel and Reset buttons for input dialogs
* Grouping options for the energy plot
* Copy, cut and paste on all tables

**Changed**

* Use desktop theme icons on Linux where appropriate
* Save window sizes and reload them on startup

**Fixed**

* Initial state `t = 0` missing from the dynamic output data
* Program terminates when GUI is started with invalid file

## Version 0.4.0
---

**Added**

* More robust static simulation by using line searching
* Contact handling for simulating recurve bows
* New output result: Grip force
* New output result: String force

**Changed**

* Don't start the simulation when draw length is smaller than brace height

## Version 0.3.0
---

**Added**

* Command line interface for starting simulations without the GUI
* Store simulation results in binary `.dat` files
* Compatibility for older `.bow` files

**Changed**

* Replace sampling time with sampling rate/frequency in the dynamics settings

## Version 0.2.0
---

**Added**

* Tree based bow editor
* 3D view of the limb geometry
* Energies in user selected plots

**Fixed**

* Ticks on opposite plot axis didn't fit together
* Crash when section height is zero
* Crash when cancel is clicked right after starting the simulation

**Removed**

* Support for MacOS

## Version 0.1.0
---

**Added**

* Platform support for Windows, Linux and MacOS
* New GUI using the Qt framework
