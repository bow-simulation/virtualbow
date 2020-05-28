# Changelog

## v0.7.1

### Fixed
* Missing libraries for VirtualBow Solver and Post on MacOS
* Static/Dynamic buttons in VirtualBow Post not working on MacOS
* Simulation failing with `Error: bad_function_call` for certain lengths of the profile curve

## v0.7

### Added
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

### Removed
* Most of the menu icons, except very specific ones
* Releases for 32 bit Windows
* AppImage releases

### Changed
* Application logo
* Compile with MinGW under Windows for better simulation performance
* The profile curve now has continuous curvature, no more jumps
* Keep the blue default color on stress plots when there is only one layer
* More ticks on the y-axis of the stress plots and others

### Fixed
* Problem that `VCRUNTIME_140_1.dll` was not found under Windows
* Short flashing of the console window when starting VirtualBow on Windows

## v0.6.1

### Fixed
* Error when trying to read the simulation output on Windows with the Python example script

## v0.6

### Added
* Support for MacOS
* Support for `rpm` based Linux distributions
* Output limb curvature

### Changed
* Use monotonic cubic splines for layer width and height interpolation
* Define the offsets of the limb centerline in terms of *setback* and *handle length* parameters

### Fixed
* Crash when loading an invalid `.bow` file from the command line
* Problem with face orientation of the limb geometry in the 3D view

## v0.5

### Added
* Possibility to define laminated bows
* Support for 64 bit Windows
* Cancel and Reset buttons for input dialogs
* Grouping options for the energy plot
* Copy, cut and paste on all tables

### Changed
* Use desktop theme icons on Linux where appropriate
* Save window sizes and reload them on startup

### Fixed
* Initial state `t = 0` missing from the dynamic output data
* Program terminates when GUI is started with invalid file

## v0.4

### Added
* More robust static simulation by using line searching
* Contact handling for simulating recurve bows
* New output result: Grip force
* New output result: String force

### Changed
* Don't start the simulation when draw length is smaller than brace height

## v0.3

### Added
* Command line interface for starting simulations without the GUI
* Store simulation results in binary `.dat` files
* Compatibility for older `.bow` files

### Changed
* Replace sampling time with sampling rate/frequency in the dynamics settings

## v0.2

### Added
* Tree based bow editor
* 3D view of the limb geometry
* Energies in user selected plots

### Removed
* Support for MacOS

### Fixed
* Ticks on opposite plot axis didn't fit together
* Crash when section height is zero
* Crash when cancel is clicked right after starting the simulation

## v0.1

### Added
* Platform support for Windows, Linux and MacOS
* New GUI using the Qt framework
