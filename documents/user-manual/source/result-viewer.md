# Result Viewer

The result viewer is used to open and visualize simulation results that are stored in `.res` files.
It is launched automatically by the model editor when a simulation has finished, but it can also be used as a standalone application.

<figure>
  <img src="images/screenshots/viewer/result-viewer.png" style="width:90%">
  <figcaption><b>Figure:</b> Screenshot of the result viewer</figcaption>
</figure>

After loading a result file you can use the buttons on the bottom right corner of the window to switch between static and dynamic results, if available.
The results themselves are organized in different tabs, which will be explained in the next sections.

Below the tabs there is a slider where you can change the current state of the bow that is being viewed, either by draw length in the static case or by time in the dynamic case.
This value applies to all of the tabs.
Drag the slider to a specific position or use the play buttons to show the simulation results as a continuous animation.
The input field and the dropdown menu next to it allow you to jump directly to special points in the results, for example where certain forces or stresses reach their maximum.

> **Note:** The physical units that are used throughout the result viewer can be changed under _Options_ - _Units_.
