The `.bow` files in this directory are used for testing the compatibility of VirtualBow with older model formats.
This is done to ensure that they can still be loaded and produce the same bow model (but not necessarily the same simulation results).
The model parameters in those files are more or less random, they are not intended to be good examples.

Each folder contains files for the same bow in different versions of the file format, starting with the format the folder is named after.
For example, the folder `v2` contains the files `v2.bow`, `v3.bow`, ... up to the current version.
Example files starting at a later version are necessary because sometimes new features are introduced that can't be tested with bow files going all the way back.
