# Model Editor

The model editor is the first thing you will see when opening VirtualBow.
Here you can design your bows, specify their physical properties and start simulations to investigate their performance.

<figure>
  <img src="images/screenshots/editor/model-editor.png" style="width:90%">
  <figcaption><b>Figure:</b> Screenshot of the model editor</figcaption>
</figure>

## Loading and saving files

Use the file menu and/or the toolbar buttons on the top to create, open and save bow models.
Bow models are stored as `.bow` files on disk, which contain all the physical parameters of the bow.
You could share those files with other users and they would be able to open and view your creations.

> **Note:** VirtualBow's goal is to keep compatibility with older `.bow` files as much as possible, so you should be able to open files that were created with older versions.
> Keep in mind though that saving will overwrite the file in the new format.

## Editing the bow model

The main view of the editor is a 3D visualization of the bow's current geometry.
Use the mouse to rotate (left button), shift (middle button) and zoom (mouse wheel) the perspective.
More view options are available through the buttons on the bottom.
Besides this 3D view, the editor interface contains three additional panels:

- **Model**: The model tree on the top left shows how the bow model is organized, with various categories of physical properties.
If an item in the model tree is selected, its details are shown in the _Properties_ and/or the _Graph_ panels.
Some categories in the tree can be edited by adding, removing or renaming items, namely _Materials_, _Layers_ and _Profile_.

- **Properties**: The property editor shows the details of the currently selected model tree item.
Here you can view and edit the physical properties of the bow model.

- **Graph**: The graph view will show any graphs/plots that are associated with the selected item in the model tree.
The graphs usually reflect the current properties and change as the properties are being edited.

The physical units that are used throughout the model editor can be changed under _Options_ - _Units_.

## Running Simulations

Simulations can be started with the _Simuate_ menu or by clicking one of the toolbar buttons.
Any changes to the bow will be saved and the VirtualBow solver invoked on the model file.

- <img src="images/icons/run-statics.svg" style="width:20; vertical-align:top"> **Statics**:
The static simulation analyzes the bow as it is being drawn from brace height to full draw.
One of the results is the force/draw curve, for example.
This mode is called static since the bow is considered to be in static equilibrium at each stage of the draw.

- <img src="images/icons/run-dynamics.svg" style="width:20; vertical-align:top"> **Dynamics**:
The dynamic simulation analyzes the bow and arrow in motion as the string is released from full draw.
It adds things like arrow speed and degree of efficiency to the results.
Since it requires the initial state of the bow at full draw, the dynamic simulation will always perform a static simulation as well.

The simulation results are stored as a `.res` file next to the model file and automatically opened in the result viewer for analysis.