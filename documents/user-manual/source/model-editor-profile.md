# Profile

The profile defines the shape of the (back of the) bow in unbraced state.

<figure>
  <img src="images/screenshots/editor/profile.png" style="width:200px">
  <figcaption><b>Figure:</b> Profile segments in the model tree</figcaption>
</figure>

If the _Profile_ category in the model tree is selected, the buttons (<img src="images/icons/list-add.svg" style="width:20; vertical-align:middle">, <img src="images/icons/list-remove.svg" style="width:20; vertical-align:middle">, <img src="images/icons/list-move-up.svg" style="width:20; vertical-align:middle">, <img src="images/icons/list-move-down.svg" style="width:20; vertical-align:middle">) can be used to add, remove and reorder segments of different types that make up the profile curve (e.g. lines, arcs and more).
The properties of each segment depend on its type and are explained below.
The resulting shape of the profile curve is shown in the _Graph_ panel.
You can use the context menu for additional options on the plot, like showing/hiding control points, curvature or adding an overlay image.

<figure>
  <img src="images/screenshots/editor/profile-plot.png" style="width:800px">
  <figcaption><b>Figure:</b> Profile plot</figcaption>
</figure>

>  **Note:** The profile curve always starts at (0, 0) and with a horizontal angle. Any offsets in the actual bow can be achieved with the parameters in [Dimensions](model-editor-dimensions.md).

## Line Segments

The line segment is simply a straight line.
Its only editable property is the length of the line.

<figure>
  <img src="images/segment-line.svg" style="width:500px">
  <figcaption><b>Figure:</b> Line segment properties</figcaption>
</figure>

## Arc Segments

The arc segment describes a circular arc with given length and radius.

> **Note:** The radius can be positive or negative, which makes the arc curve into different directions.
It can also be zero, in this case the arc turns into a straight line.

<figure>
  <img src="images/segment-arc.svg" style="width:400px">
  <figcaption><b>Figure:</b> Arc segment properties</figcaption>
</figure>

## Spiral Segments

The spiral segment describes an [Euler spiral](https://en.wikipedia.org/wiki/Euler_spiral), a curve that linearly transitions between two curvatures.
It is therefore well suited for making smooth transitions between lines and arcs or arcs of different radius.
It is defined by a starting radius, and end radius and a length.

> **Note:** The start and end radius can be positive, negative or zero (similar to the arc segment).

<figure>
  <img src="images/segment-spiral.svg" style="width:400px">
  <figcaption><b>Figure:</b> Spiral segment properties</figcaption>
</figure>

## Spline Segments

The spline segment interpolates a table of `(x, y)` values with a cubic spline.
The values are measured relative to the starting point of the segment/the end point of the previous segment.

<figure>
  <img src="images/segment-spline.svg" style="width:400px">
  <figcaption><b>Figure:</b> Spline segment properties</figcaption>
</figure>