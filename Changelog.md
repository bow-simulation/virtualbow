# Changelog

## 0.6.0 (10.08.2018)

### Added
* Support for MacOS
* Support for `rpm` based Linux distributions
* New output result: Limb curvature

### Changed
* Use monotonic cubic splines for layer width and height interpolation
* Define the offsets of the limb centerline in terms of *setback* and *handle length* parameters

### Fixed
* Crash when loading an invalid `.bow` file from the command line
* Problem with face orientation of the limb geometry in the 3D view

## 0.5.0 (18.03.2018)

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

## 0.4.0 (04.11.2017)

### Added
* More robust static simulation by using line searching
* Contact handling for simulating recurve bows
* New output result: Grip force
* New output result: String force

### Changed
* Don't start the simulation when draw length is smaller than brace height

## 0.3.0 (04.09.2017)

### Added
* Command line interface for starting simulations without the GUI
* Store simulation results in binary `.dat` files
* Compatibility for older `.bow` files

### Changed
* Replace sampling time with sampling rate/frequency in the dynamics settings

## 0.2.0 (06.08.2017)

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

## 0.1.0 (25.12.2016)

### Added
* Platform support for Windows, Linux and MacOS
* New GUI using the Qt framework
