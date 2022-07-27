# Width

This defines the limb's width along its length.
The width is the same for all layers of the bow.

<figure>
  <img src="images/screenshots/editor/width.png" style="width:800px">
  <figcaption><b>Figure:</b> Width properties</figcaption>
</figure>

The width distribution is defined by a table of relative length and width values.
Each table row must contain a relative length along the limb (from 0% to 100%) and the corresponding width.
The actual width distribution is constructed as a smooth curve (monotone cubic spline) passing through the supplied values and is shown in the _Graph_ panel.